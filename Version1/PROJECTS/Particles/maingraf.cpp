////////////////////////////////////////////////////////////////////////////////
//		     	Szablon prostego programu symulacyjnego z cz�steczkami
//                   (przykladowy program SYMSHELL'a) 
//------------------------------------------------------------------------------
// Losowanie przy�piesze� jest z rozk�adu "normalnego" w ka�dym wymiarze
// A potem mo�na w��czy� zmiany przyspiesze� :-)
//------------------------------------------------------------------------------
// Prosta obs�uga grafiki, ale z odtwarzaniem ekranu i obslug� zdarzen.
// POD Dev-Cpp potrzebne s� dwie bibloteki:
// ".../Dev-Cpp/lib/libgdi32.a" oraz "...Dev-Cpp/lib/libcomdlg32.a"
////////////////////////////////////////////////////////////////////////////////

#include "symshell.h"
#include <stdio.h> //Wyj�cie na konsole a la j�zyk C - printf(....)
#include <math.h>  //B�dzie potrzebne troch� matematyki sqrt() itp.
#include <fstream>

#define NAZWAMODELU  "Symulacja cz�steczkowa" //U�ycie define u�atwia montowanie sta�ych �a�cuch�w
//Wyj�ciowy rozmiar �wiata i "ekranu" symulacji
const unsigned size=400;

//Liczba cz�steczek
const unsigned number=50;

//Typ reprezentuj�cy cz�steczk�
struct Particle
{
  double X,Y;//,Z  Po�o�enie
  double vX,vY;//,vZ Sk�adowe pr�dko�ci
  double aX,aY;//,aZ Sk�adowe przy�pieszenia     
};

Particle World[number];//Tablica wszystkich cz�steczek �wiata        
double TimeStep=0.001; //D�ugo�� kroku czasu (musi by� ma�a, mniejsza ni� 1
                       //gdy� jednostk� odleg�o�ci jest piksel obrazu)
//WYgodniej mie� tutaj
const int SRANDV=123; //tylko 0 oznacza losowanie
const int DELA=0;//Jak d�ugie oczekiwanie w obr�bie p�tli zdarze�
const int VISUAL=10;//Co ile krok�w symulacji odrysowywa� widok
unsigned step_counter=0;//Licznik realnych krok�w modelu

double Rozklad(unsigned ile_iteracji)
//Dzwonowy rozk�ad zmiennej - tym lepszy im wi�cej iteracji
{
  double wynik=0;
  for(unsigned i=0;i<ile_iteracji;i++)
     wynik+=rand()/double(RAND_MAX);//Kolejne decyzje
  wynik/=ile_iteracji;
  return wynik;
}

inline //G��wnie chodzi o struktur� kodu, wi�c nie chcemy p�aci� za to szybko�ci�
void init_particle(Particle& P)
{
   //TODO: inicjalizacja z centraln� mas� i pr�dko�ciami "w dysku"
   P.X=size/4+Rozklad(6)*size/2;//Trzeba je jako� rozproszy�
   P.Y=size/4+Rozklad(6)*size/2;//na pocz�tek
   /*=P.Z=size/2.0;*/ //Z wersji gdzie wszystko startuje z punktu
   P.vX=1.05*(Rozklad(6)-0.5);//Mo�na te� dac jakie� 
   P.vY=1.05*(Rozklad(6)-0.5);//losowe pr�dko�ci, ale ma�e
   /*=P.vZ=0.0;*/ //Poczatkowe pr�dko�ci by�y zerowe
   P.aX=0;//za to przyspieszenia na pewno
   P.aY=0;//b�d� si� liczy� same
   //P.aZ=rand()/double(RAND_MAX);
   //printf("A=(%g %g)\n",P.aX,P.aY);//DEBUG
}

void init_world()
//Funkcja do zapoczatkowania �wiata
{
   if(SRANDV)
   srand(SRANDV);//Powtarzanie przebiegu
   else
   srand(time(NULL));//Je�li nie chcemy powtarza� za kazdym razem tego samego
   
   for(unsigned i=0;i<number;i++)
        init_particle(World[i]);
}

inline void change_particle(Particle& P)
//Oddzia�ywania na cz�stek�, lub jej w�asne decyzje co ma robi�
{  //By�y losowe zmiany...
   /*P.aX+=0.01*(Rozklad(13)-0.5);
   P.aY+=0.01*(Rozklad(13)-0.5); 
   */
   //Jak zaimplementowa� przyci�ganie? UPROSZCZONE
   //Musi by� obliczone oddzia�ywanie od ka�dej innej cz�steczki
   P.aX=P.aY=0;//Liczymy od nowa
   for(unsigned i=0;i<number;i++)//TO JEST KOSZTOWNE!
   if((World+i)!=&P) //Por�wnanie po adresach bo nie znamy "i" tej cz�steczki
   {
       double dX=P.X-World[i].X;//X-wa sk�adowa odleg�o�ci
       double dY=P.Y-World[i].Y;//Y-wa sk�adowa odleg�o�ci
       double Dkw=dX*dX+dY*dY;//Kwadrat odleg�o�ci
       if(Dkw>0.0001)//Tylko wtedy jeste�my w stanie liczy�. Mo�e niby by� 0, ale...
       {
        double D=sqrt(Dkw);
        double a=1;//G*M*m , bo na razie zawsze tak samo
        a/=Dkw; //bo GMm/r2 , warto�� przy�pieszenia spada z kwadratem odleg�o�ci
        //Ale teraz mamy problem, jak to przy�pieszenie roz�o�y� na sk�adowe
        double aX=a*dX/D;//Zmiana przy�pieszenia jakby z proporcji
        double aY=a*dY/D;
        P.aX-=aX;
        P.aY-=aY;
       }
       else
       printf("bec! ");//... a w og�le to jest zderzenie, z pewn� dok�adno�ci�   
       //TODO: Obs�uga zderze� i r�nych mas powsta�ych z tego powodu 
   }
}


inline void accelerate_particle(Particle& P)
//Fizyka dzia�ania przy�pieszenia w bardzo ma�ej chwili czasu
{
   P.vX+=TimeStep*P.aX;
   P.vY+=TimeStep*P.aY;
   //P.vZ=TimeStep*P.aZ;
   //printf("V=(%g %g)\n",P.vX,P.vY);//DEBUG
}
 
inline void move_particle(Particle& P)
//Fizyka przesuwania w bardzo ma�ej chwili czasu
{
   P.X+=TimeStep*P.vX;
   P.Y+=TimeStep*P.vY;
   //P.Z+=TimeStep*P.vZ;
   //printf("Poz=(%g %g)\n",P.X,P.Y);//DEBUG
}

void single_step()
//Funkcja robi�ca jeden krok symulacji - raczej bardzo ma�y krok czasu 
{
    //Co� mo�e zmienia� przy�pieszenia ( przyci�gania, odpychania, decyzje! )
    for(unsigned i=0;i<number;i++)
            change_particle(World[i]);
    
    //Przy�pieszenia zmieniaj� pr�dko�ci
    for(unsigned i=0;i<number;i++)
            accelerate_particle(World[i]);
            
    //Pr�dko�ci zmieniaj� po�o�enia
    for(unsigned i=0;i<number;i++)
            move_particle(World[i]);
    
    //I zrobione:
    step_counter++;
}

void stats()
//Funkcja do obliczenia statystyk
{
  //TODO: ... Ile cia� jest, rozk�ad mas
}


//Do wizualizacji obs�ugi zdarze�
const char* CZEKAM="Czekaj i patrz! "; //Monit w p�tli zdarze�
int xmouse=10,ymouse=10;//Pozycja ostatniego "kliku" myszy 

//Kilka deklaracja zapowiadaj�cych inne funkcje obs�uguj�ce model
void replot(); //Funkcja odrysowyj�ca
void read_mouse(); // Obsluga myszy - uzywa� o ile potrzebne
void write_to_file(); // Obsluga zapisu do pliku - u�ywa� o ile potrzebne
void screen_to_file(); //Zapis ekranu do pliku (tylko Windows!)

void replot()
//Rysuje co� na ekranie
{
   //clear_screen();
   //Musimy wiedzie� jaki mamy aktualnie ekran
   int width=screen_width();
   int heigh=screen_height();
   //Petla po wszystkich cz�steczkach
   for(unsigned i=0;i<number;i++)
   {
      int x=World[i].X;
      int y=World[i].Y;
      //int z - nie u�ywamy na razie
      //Czy si� mie�cimy w ekranie?
      if(0<=x && x<=width
      && 0<=y && y<=heigh)
      {
         int c=56+(i*63)%200;//Zeby s�siednie mia�y inne kolory 
         plot(x,y,c);
      }
   }
  printc(width/2,heigh-char_height('0'),128,255,"%06u  ",step_counter);//Licznik krok�w    
}


int main(int argc,const char* argv[])//Potrzebne s� paramatry wywo�ania programu
{
 fix_size(0);       // Czy udajemy �e ekran ma zawsze taki sam rozmiar?
 mouse_activity(0); // Czy mysz b�dzie obs�ugiwana?   
 buffering_setup(1);// Czy b�dzie pamieta� w bitmapie zawartosc ekranu? 
 shell_setup(NAZWAMODELU,argc,argv);// Przygotowanie okna z u�yciem parametr�w wywo�ania
 init_plot(size,size,0,1);/* Otwarcie okna SIZExSIZE pixeli + 1 wiersz znakow za pikselami*/

 // Teraz mo�na rysowa� i pisac w oknie 
 init_world();
 replot();
 flush_plot();	// Ekran lub bitmapa po inicjalizacji jest gotowa 

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
       //case 'c': //TODO: czyszczeni/nie czyszczenie ekranu
       //case 'k': //TODO:centrowanie
       //case 'ijkm'://TODO przesuwanie pola widzenia
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
      
       if(loop%3==0)
        printc(0,screen_height()-char_height('C'),128,255,CZEKAM);
       else
        printc(0,screen_height()-char_height('C'),255,128,CZEKAM);
    
       flush_plot();// gotowe  - i wlasciwie co chwila wywolywane 
       delay_ms(DELA);//Wymuszenie oczekiwania - �eby p�tla nie zjada�a ca�ego procesora  
       }
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
     std::ofstream out(NazwaPliku); //Nazwa na razie ustalona z g�ry
     //TODO - funkcja powinna zapisa� wyniki modelu do pliku zamiast wyrysowywa� na ekranie
     //Format - tabela liczb odpowieniego typu rozdzielanych tabulacjami
     //out<<"L i c z b y:\n"<<a[]<<'\t'<<std::endl;
     
     out.close();
}

void screen_to_file()
//Zapis ekranu do pliku (tylko Windows!)
{
   char bufor[255];//Tymczasowe miejsce na utworzenie nazwy pliku
   sprintf(bufor,"%s%06u",NAZWAMODELU,step_counter);//Nazwa + Numer kroku na 6 polach
   dump_screen(bufor);
}
