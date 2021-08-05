////////////////////////////////////////////////////////////////////////////////
//  Szablon automatu kom�rkowego zastosowany do uproszczonego pomys�u dyfuzji
//                   (przykladowy program SYMSHELL'a) 
//------------------------------------------------------------------------------
// Prosta obs�uga grafiki, ale z odtwarzaniem ekranu i obslug� zdarzen.
// VC++ linkuje biblioteki Windows automatycznie
// POD Dev-Cpp potrzebne s� dwie bibloteki:
// ".../Dev-Cpp/lib/libgdi32.a" oraz "...Dev-Cpp/lib/libcomdlg32.a"
////////////////////////////////////////////////////////////////////////////////
#define MULTITR (1)  //Je�li chcemy u�y� wielow�tkowo�ci

#include <cstdio>    //Wyj�cie na konsole a la j�zyk C - printf(....)
#include <cmath>     //Troch� funkcji matematycznych z j�zyka C
#include <ctime>
#include <cassert>
#include <fstream>   // C++ strumienie plikowe
#include <iostream>  // C++ obs�uga konsoli
#ifdef MULTITR
#include <thread>    //Obsluga w�tk�w
const unsigned max_threads=16;//Maksymalna przewidziana liczba w�tk�w
unsigned prefered_threads=3; //Ile w�tk�w domy�lnie?
//Ziarno dla w�tkowego generatora liczb pseudolosowych
__declspec(thread) //thread_local z C++11 powinno dzia�a� ale w VC++2012 jednak tego nie robi
unsigned long  myholdrand=1;
int myrand()//Na wz�r rand() Microsoftu
{
    return( ((myholdrand = myholdrand * 214013L
            + 2531011L) >> 16) & 0x7fff );
}
#endif
#include "SYMSHELL/symshell.h"
#include "INCLUDE/cticker.hpp"
#include "INCLUDE/optParam.hpp"
cticker MyCPUClock;         //Czas od startu programu do liczenia "�redniego czasu kroku brutto"

#define NAZWAMODELU  "2x2 gas v1.0mt " //U�ycie define a nie const char* u�atwia montowanie sta�ych �a�cuch�w
//Wyj�ciowy rozmiar �wiata i "ekranu" symulacji
const int MAXSIZE=1000;
int size=500;//Raczej powinno by� parzyste!

//Do wizualizacji obs�ugi zdarze�
const char* CZEKAM="?>"; //Monit w p�tli zdarze�
const int DELA=0;//Jak d�ugie oczekiwanie w obr�bie p�tli zdarze�
unsigned VISUAL=1;//Co ile krok�w symulacji odrysowywa� widok
int xmouse=10,ymouse=10;//Pozycja ostatniego "kliku" myszy 

unsigned RANDOM_SEED=time(NULL); //Zarodek generatora pseudolosowego 
unsigned DENSITY=(size*size)/100;//Musi by� tyle, �eby by�y miejsca z kom�rkami obok siebie
                        //TODO: manipulacja g�sto�ci�
                        
wbrtm::OptionalParameterBase* Parameters[]={ //sizeof(Parameters)/sizeof(Parameters[])
new wbrtm::ParameterLabel("PARAMETERS FOR SINGLE SIMULATION"),
new wbrtm::OptionalParameter<unsigned>(DENSITY,1,10000,"DENSITY","How many particles"),
new wbrtm::OptionalParameter<int>(size,0,MAXSIZE,"SIDELEN","Side of the world"),
new wbrtm::ParameterLabel("TECHNICAL PARAMETERS"),
new wbrtm::OptionalParameter<unsigned>(VISUAL,1,10000,"VISUAL","How many steps between visualisation"),
#ifdef MULTITR
new wbrtm::OptionalParameter<unsigned>(prefered_threads,1,max_threads,"THREADS","How many threads"),
#else
new wbrtm::OptionalParameter<unsigned>(RANDOM_SEED,1,0x01FFFFFF,"RANDSEED","Use, if you want the same simulation many times. Do not work with multithreated version"),//Zasiewanie liczby losowej
#endif
new wbrtm::ParameterLabel("END OF LIST")
};    


//Klasyczny automat kom�rkowy zmienia stan wszystkich kom�rek "jednocze�nie"
//co w praktyce oznacza, �e potrzebna jest jedna tablica na stan aktualny 
//i jedna na przysz�y.
typedef unsigned char WorldType[MAXSIZE][MAXSIZE]; //Zadeklarowanie takiego typu pomaga obej��
                                             //pokr�tny spos�b deklaracji wska�nika do tablicy
//unsigned char �eby by�o od 0 do 255, bo typ char bywa te� "signed" (zaleznie od kompilatora)
//Dodatkowa zaleta �e wystarczy zmieni� ten typedef i zmieniaj� si� wszystkie zmienne 
//tego typu.

WorldType World1;//Pierwsze tablica �wiata - wyzerowana na poczatku bo globalna
WorldType World2;//Druga tablica �wiata - j.w.
WorldType* World=(&World1);//Aktualna tablica �wiata, a w�a�ciwie adres do niej

unsigned step_counter=0;//Licznik realnych krok�w modelu

void init_world()
//Funkcja do zapoczatkowania �wiata
//Wsypujemy troch� jedynek w losowe miejsca u�ywaj�c rand() z C RTL
{
      srand(RANDOM_SEED); //Inicjacja generatora liczb pseudolosowych     
      for(unsigned k=0;k<DENSITY;k++) 
      {
          unsigned i=rand() % size; //% operacja reszta z dzielenia
          unsigned j=rand() % size;//czyli "modulo". Chmmm... *
          World1[i][j]=1; //Gdzieniegdzie co� ma by�...        
      }
// * Robienie modulo z wynikiem funkcji rand() jest odradzane przez znawc�w :-)
//   Ale przy tak ma�ym "swiecie" mo�e nie zobaczymy problemu      
}

//Fukcja do zamykania �wiata w torus
inline //<-- To funkcja "rozwijana w kodzie" 
unsigned BezpiecznyOffset(int offset,unsigned start,unsigned WYMIAR)
{
   //Operacje na typie BEZ ZNAKU! G��wne za�o�enie - nigdy nie przej�� poni�ej zera!!!!!
   unsigned pom=WYMIAR;//Bepieczna "g�ebia"
   pom=pom+start+offset;//Problem by�by tylko gdy ofset ujemny i ABS(offset)>WYMIAR+start
                        //... ale to by by�a du�a z�o�liwo�� :-) 
                        //Ale teraz na pewno wyjchodzi poza WYMIAR od strony dodatniej
   pom=pom % WYMIAR;    //To za�atwia "reszta z dzielenia" kt�r� i tak by�my robili
   return pom;                     
}

unsigned char Rules[16][7]={
{ 0, 0, 0, 0, 0, 0, 0},// --  0000 -> 0x0
					   // --
{ 1, 8, 4, 2, 1, 8, 4},// --  0001 --> 0x1
				       // -x
{ 1, 4, 2, 1, 8, 4, 2},// --  0010 --> 0x2
				       // x-
{ 2,12,10, 9, 6, 5, 3},// --  0011 --> 0x3
				       // xx
{ 1, 2, 1, 8, 4, 2, 1},// -x  0100 --> 0x4
				       // --
{ 2,10, 9, 6, 5, 3,12},// -x  0101 --> 0x5
				       // -x
{ 2, 9, 6, 5, 3,12,10},// -x  0110 --> 0x6
				       // x-
{ 3,14,13,11, 7,14,13},// -x  0111 --> 0x7 
				       // xx
{ 1, 1, 8, 4, 2, 1, 8},// x-  1000 --> 0x8
				       // --
{ 2, 6, 5, 3,12,10, 9},// x-  1001 --> 0x9
				       // -x
{ 2, 5, 3,12,10, 9, 6},// x-  1010 --> 0xa czyli 10
				       // x-
{ 3,13,11, 7,14,13,11},// x-  1011 --> 0xb czyli 11
				       // xx
{ 2, 3,12,10, 9, 6, 5},// x-  1100 --> 0xc czyli 12
				       // x-
{ 3,11, 7,14,13,11, 7},// xx  1101 --> 0xd czyli 13
				       // -x
{ 3, 7,14,13,11, 7,14},// xx  1110 --> 0xe czyli 14
				       // x-
{ 4,15,15,15,15,15,15} // xx  1111 --> 0xf czyli 15
				       // xx
};
//DEFINICJA FUNKCJI RegulaIZmiana
//B�d�cej w�a�ciw� implementacj� automatu - tu si� ustala stan aktualny oraz wybiera nast�pny
inline //<-- To funkcja "rozwijana w kodzie"
void RegulaIZmiana(			unsigned i, //Wiersz startowej komorki bloku
							unsigned j, //Kolumna startowej komorki 
							WorldType& SW,//Z jakiego swiata?
							WorldType& TW //Do jakiego �wiata
							//,unsigned& rstate //stan generatora rand_r
             )
{
	unsigned le=(j+1)%size; //Zamkni�cie w torus
	unsigned bo=(i+1)%size; //jest uproszczone...
	unsigned char old=(SW[i][ j]&1)*8 + (SW[bo][ j]&1)*4
					 +(SW[i][le]&1)*2 + (SW[bo][le]&1)*1;
															assert(old<16);		
	//unsigned randVal=4;       //Test bez rand()
	//unsigned randVal=rand_r(&rstate);//Niestety ju� "obsolete" i nie ma jej w stdlib
#ifdef MULTITR
	unsigned randVal=myrand(); //u�ywa w�tkowo specyficznego stanu 
#else
	unsigned randVal=rand();	//Czy rand() jest jako� zabezpieczone wzgl�dem w�tk�w? OG�LNIE W�TPIE. W  MSVC++ nie
#endif

    unsigned char nex=old==0?0:old==15?15:Rules[old][1+randVal%6];//Sprawdzenie stanu, ale 0 i 15 nie maj� szans na zmian�
															assert(nex<16);
															assert(Rules[old][0] == Rules[nex][0]);
	TW[ i][ j]= nex&8?1:0; 
	TW[bo][ j]= nex&4?1:0;
	TW[ i][le]= nex&2?1:0; 
	TW[bo][le]= nex&1?1:0;
}

#ifdef MULTITR
void doMove(int StartLine,int EndLine,int Parity, WorldType* pSW,WorldType* pTW)
{
	 //printf("%u %u\n",StartLine,EndLine);fflush(stdout);
	 myholdrand=time(NULL)+int(&EndLine);//Jakie� �r�nicowane �r�d�o ziarna
	 for(unsigned i=StartLine;i<=EndLine;i+=2)
	 {                                                              assert(i<size);
	   for(unsigned j=Parity;j<size;j+=2)      
		RegulaIZmiana(i,j,*pSW,*pTW);
	 }
}
#endif

void single_step()
//Funkcja robi�ca jeden SYNCHRONICZNY krok symulacji
{
    //Ustalenie co jest stare a co nowe tym razem
    WorldType* OldWorld=NULL;//DLa pewno�ci pusty wska�nik 
    unsigned parity=step_counter%2;
	if(parity==0)//Dla wyj�cia z kroku 0 i parzystych
    { 
      OldWorld=(&World1);World=(&World2);//�wiat pierwszy jest �r�d�em danych a nowym �wiatem (wynikiem) b�dzie drugi �wiat
    }
    else //Dla wyj�cia z kroku 1 i nieparzystych
    { 
      OldWorld=(&World2); World=(&World1);//�wiat drugi jest �r�d�em danych a nowym �wiatem bedzie ponownie pierwszy �wiat
    }

//Teraz w�a�ciwe wyliczenie nowego stanu automatu
#ifdef MULTITR
	std::thread theThreads[max_threads];  
	int lines_per_thr=(size/2)/prefered_threads;//Tylko co druga linia mo�e by� startow�
	int rest_lines=(size/2)%prefered_threads;//W�tek pierwszy dostaje ca�� reszt�
	int fl=parity;
	int ll=fl+(rest_lines+lines_per_thr)*2-1;//ll to indeks ostatniej lini  
	for(int t=0;t<prefered_threads;t++)
	{
		theThreads[t]=std::thread(doMove,fl,ll,parity,OldWorld,World);//T1: doMove(parity,size/pref-1,parity,OldWorld,World);
		fl=ll+1;ll=fl+lines_per_thr*2-1;
		//if(ll>=size) ll=size-1;//To ostatnie mo�liwa linia 
	}
	//G��wny b�dzie sobie czeka�
	for(int t=0;t<prefered_threads;t++) 
		theThreads[t].join();
#else
 	for(unsigned i=parity;i<size;i+=2)
       for(unsigned j=parity;j<size;j+=2)
			RegulaIZmiana(i,j,*OldWorld,*World);
#endif
    step_counter++;
}

//Statystyki i ich liczenie oraz wy�wietlanie
unsigned alife_counter=0;//Licznik �ywych kom�rek - do u�ycie te� w wy�wietlaniu wi�c globalny

#ifdef MULTITR
void doStat(int StartLine,int EndLine,unsigned& Summ)
{
	//printf("%u %u\n",StartLine,EndLine);fflush(stdout);
	unsigned tmp=0;
	for(int y=StartLine;y<=EndLine;y++)//Tylko wskazana cz�� �wiata
	  for(int x=0;x<size;x++)//World jest wska�nikiem na tablic�, wi�c trzeba go u�y� jak 
           if((*World)[y][x]!=0)     //wska�nika, a dopiero potem jak tablicy. Nawias konieczny.
             tmp++;
	Summ=tmp;
}
#endif

void statistics()
//Funkcja do obliczenia statystyk
{
	 alife_counter=0;
#ifdef MULTITR
	//const unsigned max_threads=16;//Maksymalna przewidziana liczba w�tk�w
	//unsigned prefered_threads=3; //Ile w�tk�w domy�lnie?
	unsigned part_alife_counters[max_threads];
	std::thread* theThreads[max_threads];  
	int lines_per_thr=size/prefered_threads;
	int rest_lines=size%prefered_threads;
	for(int fl=0,ll=lines_per_thr-1,t=0;t<prefered_threads;t++)//ll to indeks ostatniej lini  
	{
		part_alife_counters[t]=0;//
		if( rest_lines-- > 0) 
			ll++; //Puki jest reszta z dzielenia, rozk�adamy j� na w�tki kt�re wystartuj� pierwsze
		theThreads[t]=new std::thread(doStat,fl,ll,std::ref(part_alife_counters[t]));
		fl=ll+1;ll+=lines_per_thr;
	}
	//G��wny b�dzie sobie czeka�
	for(int t=0;t<prefered_threads;t++)
	{
		theThreads[t]->join();
		alife_counter+=part_alife_counters[t];//Musi by� po join!
		delete theThreads[t];
	}
#else
    for(int x=0;x<size;x++)
     for(int y=0;y<size;y++)  
      {          //World jest wska�nikiem na tablic�, wi�c trzeba go u�y� jak 
                 //wska�nika, a dopiero potem jak tablicy. Nawias konieczny.
           if((*World)[y][x]!=0)    
             alife_counter++;
      }
#endif
    printc(size/2,size,200,64,"%06u  ",alife_counter);//Licznik krok�w    
   // std::cout<<step_counter<<"      \t"<<alife_counter<<std::endl;
}

//TODO 3: zapis do pliku...

//Kilka deklaracja zapowiadaj�cych inne funkcje obs�uguj�ce model
void replot(); //Funkcja odrysowyj�ca
void read_mouse(); // Obsluga myszy - uzywa� o ile potrzebne
void write_to_file(); // Obsluga zapisu do pliku - u�ywa� o ile potrzebne
void screen_to_file(); //Zapis ekranu do pliku (tylko Windows!)

void replot()
//Rysuje co� na ekranie
{
  for(int x=0;x<size-1;x++)
    for(int y=0;y<size-1;y++)  
      {          //World jest wska�nikiem na tablic�, wi�c trzeba go u�y� jak 
                 //wska�nika, a dopiero potem jak tablicy. Nawias konieczny.
        unsigned z=(*World)[y][x]*200;//Spodziewana warto�� to 0 lub 1
        z%=256; //�eby nie przekroczy� kolor�w
        //z%=512; //Albo z szaro�ciami
        plot(x,y,z);
      }
  printc(size/5,size,128,255,"%06u MstT:%g  ",step_counter,(double)MyCPUClock/step_counter);//Licznik krok�w    
  //Ostatnie polozenie kliku - bia�y krzy�yk   
  //line(xmouse,ymouse-10,xmouse,ymouse+10,255);
  //line(xmouse-10,ymouse,xmouse+10,ymouse,255);
}


int main(int argc,const char* argv[])//Potrzebne s� paramatry wywo�ania programu
{
 printf("Model \"%s\". File version %s\n",NAZWAMODELU,__TIMESTAMP__);
 if(wbrtm::OptionalParameterBase::parse_options(argc,argv,Parameters,sizeof(Parameters)/sizeof(Parameters[0])))
 {
			exit(222);
 }
#ifdef MULTITR
 printf("Number of hardware concurent contexts: %d\n",std::thread::hardware_concurrency());
 printf("Default number of threads: %d\n\n",prefered_threads); //Ile w�tk�w domy�lnie?
 myholdrand=2;     //INICJACJA myrand() dla g��wnego w�tku - pewnie nieistotna
#endif
 fix_size(1);       // Czy udajemy �e ekran ma zawsze taki sam rozmiar?
 mouse_activity(0); // Czy mysz b�dzie obs�ugiwana?   
 buffering_setup(1);// Czy b�dzie pamieta� w bitmapie zawartosc ekranu? 
 shell_setup(NAZWAMODELU,argc,argv);// Przygotowanie okna z u�yciem parametr�w wywo�ania
 init_plot(size,size,0,1);/* Otwarcie okna SIZExSIZE pixeli + 1 wiersz znakow za pikselami*/

 // Teraz mo�na rysowa� i pisac w oknie 
 init_world();  //Tu jest te� srand();
 replot();
 statistics();
 flush_plot();	// Ekran lub bitmapa po inicjalizacji jest gotowa 
 screen_to_file();//ODKOMENTOWA� jak chcemy materia� do filmu

 bool not_finished=true;//Zmienna steruj�ca zako�czeniem programu
 unsigned loop=0;    //Do zliczania nawrot�w p�tli zdarze�
 while(not_finished) //P�TLA OBS�UGI ZDARZE�
    { 
      int pom; //NA ZNAK Z WEJ�CIE OKNA GRAFICZNEGO
      loop++; 
      if(input_ready()) //Czy jest zdarzenie do obs�ugi?
      {
       pom=get_char(); //Przeczytaj nades�any znak
       switch(pom)
       {
       case 'd': screen_to_file();break; //Zrzut grafiki          
       case 'p': write_to_file();break;//Zapis do pliku tekstowego
       case '\r': replot();break;//Wymagane odrysowanie
       case '\b': read_mouse();break;//Jest zdarzenie myszy
       case EOF:  //Typowe zako�czenie
       case  27:  //ESC
       case 'q':  //Zako�czenie zdefiniowane przez programiste
       case 'Q': not_finished=false;break;             
       default:
            print(0,screen_height()-char_height('N'),"Nie wiem co znaczy %c [%d] ",pom,pom);
            printf("Nie wiem co znaczy %c [%d] ",pom,pom);
            flush_plot();	// Grafika gotowa   
            break;
        }
      }
      else //Symulacja - jako akcja na wypadek braku zdarze� do obslugi
      {
       single_step(); //Nast�pny krok
       if(step_counter%VISUAL==0) //Odrysuj gdy reszta z dzielenia r�wna 0
       {
             replot();
             statistics(); //Uwaga nie "stat" bo to funkcja z RTL j�zyka C!      
             if(loop%3==0)
               printc(0,screen_height()-char_height('C'),128,255,CZEKAM);
             else
               printc(0,screen_height()-char_height('C'),255,128,CZEKAM);
             flush_plot();// gotowe  - i wlasciwie co chwila wywolywane 
             //screen_to_file();//ODKOMENTOWA� jak chcemy materia� do filmu
       }
      
       delay_ms(DELA);//Wymuszenie oczekiwania - �eby p�tla nie zjada�a ca�ego procesora  
      } 
    }
 	    
 printf("Wykonano %d obrotow petli.\nDo widzenia!\n",loop);
 close_plot();/* Zamykamy okno - po zabawie */
 printf("Do widzenia!\n");
 return 0;
}

void read_mouse()
//Procedura obslugi myszy
{ 
   int xpos,ypos,click;
   if(get_mouse_event(&xpos,&ypos,&click)!=-1)//Operator & - pobranie adresu
   {
      xmouse=xpos;ymouse=ypos;
      //TODO - zaimplementowa� je�li potrzebne                                   
      //...
   }                      
}

void write_to_file()
{
     const char* NazwaPliku=NAZWAMODELU ".out";//U�ywamy sztuczki ze zlepianiem sta�ych 
                                        //�a�cuchowych przez kompilator
     std::cout<<"Zapis stanu do pliku \""<<NazwaPliku<<'"';
     std::ofstream out(NazwaPliku); //Nazwa na razie ustalona z g�ry
     //TODO - funkcja powinna zapisa� wyniki modelu do pliku zamiast wyrysowaywa� na ekranie
     //Format - tabela liczb odpowieniego typu rozdzielanych tabulacjami
     //out<<"L i c z b y:\n"<<a[]<<'\t'<<std::endl;
     for(int x=0;x<size;x++)
     {
      for(int y=0;y<size;y++)  
      {          //World jest wska�nikiem na tablic�, wi�c trzeba go u�y� jak 
                 //wska�nika, a dopiero potem jak tablicy. Nawias konieczny.
        unsigned z=(*World)[y][x];//Spodziewana warto�� to 0 lub 1
        out<<z<<'\t';
      }
      out<<'\n';
     }
     out.close();
     std::cout<<std::endl;
}

void screen_to_file()
//Zapis ekranu do pliku (tylko Windows!)
{  
   char bufor[255];//Tymczasowe miejsce na utworzenie nazwy pliku
#ifdef _MSC_VER /*MSVC*/
   _snprintf_s(bufor,255,"%s%06u",NAZWAMODELU,step_counter);//Nazwa + Numer kroku na 6 polach
#else
   sprintf(bufor,"%s%06u",NAZWAMODELU,step_counter);//Nazwa + Numer kroku na 6 polach
#endif
   std::cout<<"Zapis ekranu do pliku \""<<bufor<<'"';
   dump_screen(bufor);
   std::cout<<std::endl;
}
