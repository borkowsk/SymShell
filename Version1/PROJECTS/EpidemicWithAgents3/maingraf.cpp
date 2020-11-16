// WIELOBOKI W SIECI EPIDEMII - program SYMSHELLA pokazuj�cy idea agenta-obiektu
////////////////////////////////////////////////////////////////////////////////
// POD Dev-Cpp potrzebne s� dwie bibloteki:
// ".../Dev-Cpp/lib/libgdi32.a" 
// oraz "...Dev-Cpp/lib/libcomdlg32.a"

#include "symshell.h"
#include "Wieloboki.hpp"
#include <iostream>
#include <fstream> //Bo plik statystyki
#include <cstdlib>
#include <cmath>
#include <cassert> //DO WY�APYWANIA KODU DZIA�AJ�CEGO NIEZGODNIE Z ZA�O�ENIAMI
using namespace std;

const int size=500; //Bok obszaru roboczego
const int iluich=40; //ILU JEST AGENT�W!
const int ilechoroby=7; //Katar leczony trwa 7 dni a nieleczony tydzie�
const int ileodpornosci=60; //Potem ju� inny szczep wirusa

//Agent epidemiczny musi mie� pozycje, stan: zdrowy, od kiedy chory, od kiedy odporny
//Mo�e mie� te� aktualny kolor, zmieniaj�cy si� wraz ze zmian� stan�w
//Musi te� mie� informacje o po��czeniach z innymi agentami bo to model sieciowy
class Agent
////////////////////////////////////////////////////////////////////////////////
{
  Wielobok  Ksztalt;
  double Xpoz;
  double Ypoz;
  ssh_color Kolor;
  unsigned Stan;//jak 0 to zdrowy, ka�da inna warto�� to dni od zara�enia
  protected:
  //Z kim agent mo�e wchodzi� w interakcje?
  int Powiazania[iluich];//Jednowymiarowa tablica - by� mo�e nadmiarowa, ale mamy du�o pami�ci
  //Trzeba jako� ustali� wagi po��cze�
  friend void generuj_siec_powiazan(); //Musi miec dost�p do tablicy po��cze� agenta
  public:
  //Konstruktor  bezparametrowy - konieczny je�li maj� by� w tablicy     
  Agent():Ksztalt( Wielobok::Ludzik(0) )//,Xpoz(0),Ypoz(0) - mog�o by by�
     {
        Xpoz=rand()%size;
        Ypoz=rand()%size;
        Ksztalt.Centruj();//�eby �rodek by� w �rodku :-)
        Ksztalt.Skaluj(2,2);//I wielko�� jaka� rozs�dna
        //Tymczasowy kolor - NIECO ZMIENIONY ZE WZGL�DU NA KOLOROWANIE EPIDEMII
        Kolor=55+rand()%145;//Bo pierwsze 50 kolor�w jest ciemne, a powy�ej 200 zarezerowane 
        //Wst�pny stan
        Stan=0;
        //Wst�pne ustawienie powi�za� obojetne czyli brak     
        for(unsigned i=0;i<iluich;i++) Powiazania[i]=0; //Zadzia�a te� gdy float!
     }
  //Destruktor - na razie nic jawnie nie robi
  virtual ~Agent(){} //ale na wszelki wypadek wirtualny 
  //- destrukcja rzadko, wi�c to w�asciwie nic nie kosztuje je�li chodzi o czas CPU
   
  //Czytanie atrybut�w. "const" dla zaznaczenia �e tylko mo�emy czyta�
  double X() const {return Xpoz;}
  double Y() const {return Ypoz;}   
  unsigned St() const {return Stan;}
  //Ustawianie atrybut�w
  void  SetXY(double X,double Y); //Po�o�enie
  void  Zaraza();   //Zara�anie - zawsze z 0 na 1, inne ignoruje        
  //Do symulacji musi miec dost�p do tablicy agent�w    
  void ZrobKrok(Agent Swiat[],unsigned ilu); 
  //Do wizualizacji        
  void Rysuj() //Rysowanie agenta
     {Ksztalt(Xpoz,Ypoz,Kolor);}    
  //Rysowanie wszystkich agent�w i po��cze� mi�dzy nimi
  friend void replot(); //Zaprzyja�niona bo musi mie� dost�p do powi�za� 

};

//G��wna dynamika modelu jest w dzia�aniach agent�w
////////////////////////////////////////////////////////////////////////////////
void Agent::Zaraza()
//Zara�anie - zawsze z 0 na 1, inne ignoruje        
{
   if(Stan==0 //Mo�e si� zarazi� jak nigdy si� nie zetkn��
   || Stan>ilechoroby//=7; //Katar leczony trwa 7 dni a nieleczony tydzie�
      +ileodpornosci//Potem ju� inny szczep wirusa   //Albo jak straci� odporno��         
      )
   {
       Stan=1;//Zara�ony, by� mo�e ponownie
       //Zmiana koloru
       Kolor=255+30; //Pow 255 odcienie szaro�ci w symshell'u 
   }      
}

void Agent::ZrobKrok(Agent Swiat[],unsigned ilu)
{
    //Post�p ewentualnej choroby lub odporno�ci (liczba dni od zara�enia)
    if(Stan>0) 
    {
     Stan++;
    //Zmiana koloru - je�li zara�ony lub odporny. Uwaga! Czas od zara�enia mo�e by� d�ugi
     if(Stan<=ilechoroby) 
     {
           Kolor=255+30+Stan*10;//Czy nie przekracza 512?
           //if(Kolor>=512) cerr<<"KolorI? "<<Kolor<<" Stan="<<Stan<<endl;//DEBUG
     }
     else
     if(Stan<=ileodpornosci) 
     {
           Kolor=200+(54*(ileodpornosci-Stan)/ileodpornosci);//od 255 do 200
           //if(Kolor>=512) cerr<<"KolorII? "<<Kolor<<" Stan="<<Stan<<endl;//DEBUG
     }
     else //Straci� odporno��, miga sobie �eby by�o wida� �e podatny ponownie
     {
          Kolor=55+(rand()%145);//Bo pierwsze 50 kolor�w jest ciemne, a powy�ej 200 zarezerowane 
          //if(Kolor>=512) cerr<<"KolorIII? "<<Kolor<<" Stan="<<Stan<<endl;;//DEBUG
     }
                                                              assert(Kolor<512);
    }
    //Wchodzi w losowe interakcje z pod��czonymi agentami  
    //i mo�e ich zarazi�. ALE KTO� MUSI BY� NA POCZ�TKU (w inicjalizuj()) ZARA�ONY
    //Wersja najprostrza: ka�dy raz ma szanse na interakcje i zara�a jak jest chory
    if(0<Stan && Stan<=ilechoroby)
    {
     unsigned partner=rand()%iluich; //Jaki� z listy
                                                         assert(partner<iluich);
     if(Powiazania[partner]!=0)
          Swiat[partner].Zaraza();
    }
}

Agent SwiatAgentow[iluich];//"KOLEKCJA" AGENT�W - NIE B�D� ZNIKA� TO MO�E BY� TABLICA 

//Reszta programu s�u�y temu, �eby te dzia�ania mioa�y sens
////////////////////////////////////////////////////////////////////////////////
void  Agent::SetXY(double X,double Y)
//Po�o�enie
{
   if(0<X && X<size) //Gdy mie�ci si� na ekranie
             Xpoz=X; //to zapisujemy na atrybucie
   if(0<Y && Y<size) //Gdy mie�ci si� na ekranie
             Ypoz=Y; //Tak samo dla Y
}

void generuj_siec_powiazan()
//Mozna zrobi� siec:
// * losow� o jakiej� g�sto�ci
// * sie� losow� o jakim� rozk�adzie wag
// * sie� regularn� o zadanej odleg�o�ci powi�za�
// * sie� ma�ych �wiat�w (regularn� + dodatkowe odleg�e po��czenia)
// * itd, itp, etc ... 
{
     //Sie� losowa o zadanym udziale powiaza�
     unsigned N=iluich;//Ile jest agent�w
     unsigned M=((N*N)-N)/2; //Ile maksymalnie powi�za� symetrycznych
     M=M*0.15;//jaki� u�amek mo�liwych - nie za du�o bo nic nie zobaczymy
                      //A w czasie epidemi wszyscy zachoruj�
     unsigned licznik=0;//zliczanie udanych po��cze�
     do
     {
       unsigned pierwszy=rand()%iluich;
       unsigned drugi=rand()%iluich;
       cout<<pierwszy<<"->"<<drugi;//DEBUG
       
       if(pierwszy!=drugi
       && SwiatAgentow[pierwszy].Powiazania[drugi]==0 //Wystarczy sprawdzi� w jednym
       )
       {
         cout<<" W="<<1;//DEBUG
         //Symetrycznie. Na razie ka�da dana zdublowana
         //ale nie trzeba b�dzie du�o zmienia� jak uznamy 
         //�e po��czenia s� jednak asymetryczne
         SwiatAgentow[pierwszy].Powiazania[drugi]=1;
         SwiatAgentow[drugi].Powiazania[pierwszy]=1;
         licznik++; //udany
       }
       cout<<endl; //DEBUG
     }while(licznik<M);
     //inne rodzaje: TODO, TODO, TODO...
}

void inicjalizacja()
//U�o�enie pocz�tkowe i inne operacje konieczne do zrobienia na pocz�tku
{
   srand(time(NULL)); //Inicjalizacja generatora       

  //Reinicjalizacja pozycji agent�w - na to czego nie by�o w konstruktorze  
   for(unsigned i=0;i<iluich;i++) 
   {
     //Pozycje w k�ku, jak to cz�sto si� stosuje przy sieciach spo�ecznych
      double alfa=((2*M_PI)/iluich)*i; 
      double x=size/2.0+cos(alfa)*size/2.34;//Promie� mniejszy 
      double y=size/2.0+sin(alfa)*size/2.34;//... �eby im g��wki nie wychodzi�y 
      SwiatAgentow[i].SetXY(x,y);
      cout<<x<<' '<<y<<endl;//DEBUG
   } 
   
   //Jaki� rodzaj sieci - moze by� zalezny od pozycji wie� tu
   generuj_siec_powiazan();
   
   //PIERWSZY START EPIDEMII
   //Inicjalne zara�anie jakiego� agenta
   ///////////////////////////////////////////////////
   SwiatAgentow[0].Zaraza();
}     

void krok()
//Zmiany u�o�enia zwi�zane z r�nymi ruchami
{       
   //Ka�dy agent mo�e co� zrobi� - efekty widoczne potem
   for(unsigned i=0;i<iluich;i++)
    SwiatAgentow[i].ZrobKrok(SwiatAgentow,iluich);          
   
   //Inny spos�b zapocz�tkowywania epidemii 
   if(rand()/double(RAND_MAX)<0.05) //Raz na 100 krok�w srednio
   {
     unsigned ktory=rand()%iluich;
     SwiatAgentow[ktory].Zaraza();
     cout<<"Nowy start epidemii u agenta "<<ktory<<endl;
   }
                                                
}

void replot()
//Procedura odrysowujaca aktualny stan
{
  clear_screen();//Czyszczenie ca�ego ekranu na wszelki wypadek
  circle(size/2,size/2,10,128);
  //W wizualizacja sieci
  unsigned x1,y1,x2,y2;
  for(unsigned i=0;i<iluich;i++)//Ka�dy agent
  {
   x1=SwiatAgentow[i].X();
   y1=SwiatAgentow[i].Y();
   for(unsigned j=0;j<iluich;j++)//ma jaki� powi�zanych
    if(SwiatAgentow[i].Powiazania[j]!=0)
    { //... linie mi�dzy agentami - na razie nieoptymalnie, 
      //bo ka�da linia dwa razy. TODO: Jak zrobi� �eby raz tylko
     x2=SwiatAgentow[j].X();
     y2=SwiatAgentow[j].Y();
     unsigned char Kol=50+rand()%200;
     line(x1,y1,x2,y2,Kol);
    }
  }
  //Wizualizacja agent�w
  for(unsigned i=0;i<iluich;i++)
    SwiatAgentow[i].Rysuj();
   
  flush_plot();	// Bitmapa gotowa 
}

ofstream OutStat;

void statystyka() //TODO - brak numeracji krok�w czasu
{ //ilu bez kontaktu, ilu chorych, ilu odpornych, ilu ponownie podatnych
  unsigned bezKontaktu=0;
  unsigned chorych=0;
  unsigned odpornych=0;
  unsigned podatnych=0;
  //Zliczanie agent�w z r�nymi stanami
  for(unsigned i=0;i<iluich;i++)
  {
    unsigned Stan=SwiatAgentow[i].St();
    if(Stan==0) bezKontaktu++;
    else
    if(Stan<=ilechoroby) chorych++;
    else
    if(Stan<=ilechoroby+ileodpornosci) odpornych++;
    else
    podatnych++; 
  }
  //Wypis stanu w specjalnej linii na tekst (ko�cowej) ekranu kraficznego
  printc(size/2,size,50,255,"S:%u I:%u R:%u rS:%u    ",bezKontaktu,chorych,odpornych,podatnych);
  if(!OutStat.is_open())//Jak nie jest otwarte to wiemy �e trzeba otworzy�
  {
     OutStat.open("epidemia.out",ios_base::app); //Ka�d� epidemie dodaje do pliku z wynikami
     time_t      szClock; //Wg. przyk�adu 
     struct tm   *newTime;//z manuala MVC++
     time( &szClock ); //Czas w sekundach "ery komputerowej"
     //cyt.:"Return the time as seconds elapsed since midnight, January 1, 1970, or -1 in the case of an error."
     newTime = localtime( &szClock ); //Przekonwertowanie na struktur�
    //Uzycie do wydrukowania
     OutStat<<asctime(newTime ); //Linia daty i czasu ma w�asne \n !!!
     //Linia nag��wk�w
     OutStat<<"Susp.\tInfr.\tReco.\treSu"<<endl;
  }
  //Zwyczajnie kolejne dane
  OutStat<<bezKontaktu<<"\t"<<chorych<<"\t"<<odpornych<<"\t"<<podatnych<<endl; 
  OutStat.flush(); //Bufor na dysk
}

//Zmienne do obs�ugi zdarze� i dalej funkcje kt�re to robi�, w tym main()
const int DELA=100;//Oczekiwanie w obr�bie p�tli
const int WAIT=10; //Co ile krok�w migac napis
const char* CZEKAM="Patrzaj ino..."; 
unsigned long loop=0;

void read_mouse() //Procedura obslugi "kliku" myszy, np. do inspekcji agenta
{ 
   int xpos,ypos,click;
   if(get_mouse_event(&xpos,&ypos,&click)!=-1)//Operator & - pobranie adresu
   {
      //.....x=xpos;
      //.....y=ypos;
      //...TODO...Zaznacz innym kolorem wszystkie po��czenia agenta
      replot();   
      //...TODO i czekaj na jaki� klik, �eby by� czas zobaczy�                                                
   }                      
}

int main(int argc,char* argv[])//Potrzebne s� paramatry wywo�ania programu
{
   mouse_activity(1);       //Czy mysz b�dzie obs�ugiwana? TAK, ale potem
   buffering_setup(1);      //Czy pamietac w bitmapie zawartosc ekranu? NIE lub TAK
   fix_size(1);	            //Czy symulowa� niezmiennosci rozmiarow okna? TAK
   shell_setup("PRAWIE AGENTOWA SYMULACJA EPIDEMII",argc,argv);//Ustalenie tytu�u i konfiguracji z linii komend
   init_plot(size,size,0,1);// size x size pixeli + 1 wiersz znakow pod spodem
   
   inicjalizacja();         //Przygotowanie paramer�w
   replot();  //Rysowanie 
   statystyka(); //Obliczanie chwilowych statystyk
   
    bool kontynuuj=true;	//Zmienna steruj�ca zako�czeniem programu
    while(kontynuuj)        //P�TLA OBS�UGI ZDARZE�
    { 
      int pom;
      loop++; //Zliczanie nawrot�w
      if(input_ready()) //Czy jest co� do obs�ugi?
      {
       pom=get_char(); //Przeczytaj nades�any znak
       switch(pom)
       {
       case '\r': replot();break;//Wymagane odrysowanie
       case '\b': read_mouse();break;//Zdarzenie myszy
       case EOF:  //Typowe zako�czenie
       case  27:  //ESC
       case 'q':  //Zako�czenie zdefiniowane przez programiste
       case 'Q': kontynuuj=false;break;             
       default:
            print(0,screen_height()-char_height('N'),"Nie wiem co znaczy %c [%d] ",pom,pom);
            flush_plot();	// Bitmapa gotowa   
            break;
        }
     cout<<"Zareagowano na kod nr "<<int(pom)<<" w nawrocie "<<loop<<endl;   
  }
  else //Akcja na wypadek braku zdarze�
  {
  krok();    //Poruszanie
  replot();  //Rysowanie  
  statystyka(); //Obliczanie chwilowych statystyk
 
  if(loop%WAIT==0)
     printc(0,screen_height()-char_height('C'),128,255,"%s %d  ",CZEKAM,loop);
  else
     printc(0,screen_height()-char_height('C'),255,28,"%s %d  ",CZEKAM,loop);       
  flush_plot();// Bitmapa po modyfikacji ju� gotowa   
  delay_ms(DELA);//Wymuszenie oczekiwania - �eby p�tla nie zjada�a ca�ego procesora  
             //I �eby si� wszystko nie dzia�o zbyt szybko!
  } 
}

cout<<"Wykonano "<<loop<<" obrotow petli.\nDo widzenia!\n"<<endl;
close_plot(); //Po zabawie - zamykam okno
return 0;
}
