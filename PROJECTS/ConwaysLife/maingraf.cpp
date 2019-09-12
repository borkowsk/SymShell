////////////////////////////////////////////////////////////////////////////////
//		     	Szablon automatu kom�rkowego np. do regu�y LIFE CONWAYA
//                   (przykladowy program SYMSHELL'a) 
//------------------------------------------------------------------------------
// Prosta obs�uga grafiki, ale z odtwarzaniem ekranu i obslug� zdarzen.
// POD Dev-Cpp potrzebne s� dwie bibloteki:
// ".../Dev-Cpp/lib/libgdi32.a" oraz "...Dev-Cpp/lib/libcomdlg32.a"
////////////////////////////////////////////////////////////////////////////////

#include "symshell.h"
#include <cstdio>    //Wyj�cie na konsole a la j�zyk C - printf(....)
#include <cmath>     //Troch� funkcji matematycznych z j�zyka C
#include <fstream>   // C++ strumienie plikowe
#include <iostream>  // C++ obs�uga konsoli

#define NAZWAMODELU  "Life a la John Conway" //U�ycie define a nie const char* u�atwia montowanie sta�ych �a�cuch�w
//Wyj�ciowy rozmiar �wiata i "ekranu" symulacji
const int size=400;

const unsigned RANDOM_SEED=time(NULL); //Zarodek generatora pseudolosowego 
unsigned DENSITY=(size*size)/10;//Musi by� tyle, �eby by�y miejsca z kom�rkami obok siebie
                        //TODO: manipulacja g�sto�ci�, zw�aszcza dla innych regu� ni� klasyczne
                        
//Klasyczny automat kom�rkowy zmienia stan wszystkich kom�rek "jednocze�nie"
//co w praktyce oznacza, �e potrzebna jest jedna tablica na stan aktualny 
//i jedna na przysz�y.
typedef unsigned char WorldType[size][size]; //Zadeklarowanie takiego typu pomaga obej��
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

//DEKLARACJA ZAPOWIADAJACA FUNKCJI Regula
//B�d�cej w�a�ciw� implementacj� automatu.
unsigned char Regula( unsigned i, //Wiersz komorki
                      unsigned j, //Kolumna komorki
                  //Z jakiego swiata?
                  WorldType* SourceWorld
             );

void single_step()
//Funkcja robi�ca jeden SYNCHRONICZNY krok symulacji
{
    //Ustalenie co jest stare a co nowe tym razem
    WorldType* OldWorld=NULL;//DLa pewno�ci pusty wska�nik 
    unsigned parity=step_counter%2;
    if(parity==0)
    { //Dla wyj�cia z kroku 0 i parzystych
      OldWorld=(&World1);//�wiat pierwszy jest �r�d�em danych
      World=(&World2);//Nowym �wiatem (wynikiem) b�dzie drugi �wiat
    }
    else
    { //Dla wyj�cia z kroku 1 i nieparzystych
      OldWorld=(&World2);//�wiat drugi jest �r�d�em danych
      World=(&World1);//Nowym �wiatem bedzie ponownie pierwszy �wiat
    }
    //Teraz w�a�ciwe wyliczenie nowego stanu automatu
    for(unsigned i=0;i<size;i++)
       for(unsigned j=0;j<size;j++)
       if(Regula(i,j,OldWorld))//UZYCIE FUNKCJI Regula
       {
         (*World)[i][j]=1;//+parity;  
       }
       else
       {
         (*World)[i][j]=0;  
       }
    
    step_counter++;
}

//Fukcja do zamykania �wiata w torus
inline //<-- To funkcja "rozwijana w kodzie" - tylko sk�adnia, bez koszt�w wywo�ania
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

//Teraz parametry do uog�lnionej regu�a Conwaya. W nawiasach warto�ci zgodne z
//pomys�em oryginalnym, ale mo�na si� pobawi� i zmieni� na co� innego.
//Jak niezyjesz to si� rodzisz przy conajmniej X(=3) s�siadach, 
//ale nie wi�cej ni� Z(=3)
//A jak �yjesz to potrzebujesz nie mniej ni� Y(=2) i nie wi�cej niz Z �eby prze�y�
const unsigned X=3,Y=2,Z=3;//Klasyka
//const unsigned X=1,Y=1,Z=1;//"Migotka"
//const unsigned X=1,Y=1,Z=8;//NIEZNISZCZALNI
//const unsigned X=2,Y=2,Z=3;//Nie lubi�cy ciasnoty
//const unsigned X=3,Y=2,Z=4;
//const unsigned X=2,Y=1,Z=6;//"LUDZKOSC" :-)
//const unsigned X=4,Y=1,Z=7;//4 p�ciowy - marne szanse ba start
//TODO: Badamy r�ne konfiguracje pod zwgl�dem zasiedlania przestrzeni 
//TODO2: Dodajemy parametr Z1 ogranicaj�cy szanse na powstanie m�odego - np.
//mlody powstaje tylko gdy ma 2 rodzic�w, ale prze�ywa te� gdy ma 3 s�siad�w

//DEFINICJA(implementacja) FUNKCJI Regula
unsigned char Regula( unsigned a,unsigned b, //Parametry mog� si� inaczej nazywa�
             WorldType* SWorld) //ni� w deklaracji zapowiadaj�cej
{
    unsigned suma=0;
    for(int i=-1;i<2;i++)
       for(int j=-1;j<2;j++)
       {  //Zwijanie TORUSA
          unsigned w=BezpiecznyOffset(i,a,size); //w pionie
          unsigned k=BezpiecznyOffset(j,b,size);//i w poziomie
          suma+=(*SWorld)[w][k]==0?0:1; //Zlicza niezerowe     
       }

    if((*SWorld)[a][b]==0)
    {
     return (X<=suma && suma<=Z? 1 : 0 );//Dla "martwych"
    }
    else //Dla �ywych
    {
     suma--;//Bo w petli wliczyli�my srodkowego!!! Tansze ni� powtarzany warunek   
     return (Y<=suma && suma<=Z? 1 : 0 );
    }
}



//Statystyki i ich liczenie oraz wy�wietlanie
unsigned alife_counter=0;//Licznik �ywych kom�rek
//TODO3: zapis do pliku...
void stats()
//Funkcja do obliczenia statystyk
{
    alife_counter=0;
    for(int x=0;x<size;x++)
     for(int y=0;y<size;y++)  
      {          //World jest wska�nikiem na tablic�, wi�c trzeba go u�y� jak 
                 //wska�nika, a dopiero potem jak tablicy. Nawias konieczny.
           if((*World)[y][x]!=0)    
             alife_counter++;
      }
    printc(size/2,size,200,64,"%06u  ",alife_counter);//Licznik krok�w    
    std::cout<<step_counter<<"      \t"<<alife_counter<<std::endl;
}


//Do wizualizacji obs�ugi zdarze�
const int DELA=0;//Jak d�ugie oczekiwanie w obr�bie p�tli zdarze�
const int VISUAL=10;//Co ile krok�w symulacji odrysowywa� widok
const char* CZEKAM="Klikaj lub patrz! "; //Monit w p�tli zdarze�
int xmouse=10,ymouse=10;//Pozycja ostatniego "kliku" myszy 

//Kilka deklaracja zapowiadaj�cych inne funkcje obs�uguj�ce model
void replot(); //Funkcja odrysowyj�ca
void read_mouse(); // Obsluga myszy - uzywa� o ile potrzebne
void write_to_file(); // Obsluga zapisu do pliku - u�ywa� o ile potrzebne
void screen_to_file(); //Zapis ekranu do pliku (tylko Windows!)

void replot()
//Rysuje co� na ekranie
{
  for(int x=0;x<size;x++)
    for(int y=0;y<size;y++)  
      {          //World jest wska�nikiem na tablic�, wi�c trzeba go u�y� jak 
                 //wska�nika, a dopiero potem jak tablicy. Nawias konieczny.
        unsigned z=(*World)[y][x]*200;//Spodziewana warto�� to 0 lub 1
        z%=256; //�eby nie przekroczy� kolor�w
        //z%=512; //Albo z szaro�ciami
        plot(x,y,z);
      }
  printc(size/3,size,128,255,"%06u  ",step_counter);//Licznik krok�w    
  //Ostatnie polozenie kliku - bia�y krzy�yk   
  //line(xmouse,ymouse-10,xmouse,ymouse+10,255);
  //line(xmouse-10,ymouse,xmouse+10,ymouse,255);
}


int main(int argc,const char* argv[])//Potrzebne s� paramatry wywo�ania programu
{
 fix_size(1);       // Czy udajemy �e ekran ma zawsze taki sam rozmiar?
 mouse_activity(0); // Czy mysz b�dzie obs�ugiwana?   
 buffering_setup(1);// Czy b�dzie pamieta� w bitmapie zawartosc ekranu? 
 shell_setup(NAZWAMODELU,argc,argv);// Przygotowanie okna z u�yciem parametr�w wywo�ania
 init_plot(size,size,0,1);/* Otwarcie okna SIZExSIZE pixeli + 1 wiersz znakow za pikselami*/

 // Teraz mo�na rysowa� i pisac w oknie 
 init_world();
 replot();
 stats();
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
             stats(); //Uwaga nie "stat" bo to funkcja z RTL j�zyka C!      
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
   sprintf(bufor,"%s%06u",NAZWAMODELU,step_counter);//Nazwa + Numer kroku na 6 polach
   std::cout<<"Zapis ekranu do pliku \""<<bufor<<'"';
   dump_screen(bufor);
   std::cout<<std::endl;
}
