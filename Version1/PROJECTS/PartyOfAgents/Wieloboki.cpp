#include <iostream> //Tylko DEBUG
#include <cassert> //Funkcja assert() s�u��ca do sprawdze� w fazie produkcji 
#include <cstring> //Bo musimy/chcemy u�y� funkcji "memcpy" - kopiowania pami�ci 
#include <cmath> //sin i cos
//#include <stdint.h>
using namespace std;
#include "Wieloboki.hpp"

// Definicja kontur�w r�nych figur
////////////////////////////////////////////
//S� "static" tzn. �e nie b�d� widoczne poza tym modu�em.
//Za to b�d� widoczne ich "opakowania" dost�pne z typu Wielobok
static ssh_point kRomb[]= 
{{0,0},{10,10},{0,20},{-10,10}};

static ssh_point kDomek[]=
{{0,0},{10,10},{10,20},{-10,20},{-10,10}};

static ssh_point kUfo[]=
{{0,0},
 {10,10},{20,10},{30,20},{20,30},{10,30},{0,40},
 {-10,30},{-20,30},{-30,20},{-20,10},{-10,10}};
 
static ssh_point kLudzik0[]=
{{0,-12},{1,-12},{2,-11},{2,-10},{1,-8},{4,-6},{4,3},{3,2},{3,-3},{3,0},{2,11},{3,11},{3,12},{0,12},{0,2},
{0,12},{-3,12},{-3,11},{-2,11},{-3,0},{-3,-3},{-3,2},{-4,3},{-4,-6},{-1,-8},{-2,-10},{-2,-11},{-1,-12}};  

static ssh_point kLudzik1[]=
{{0,-12},{1,-12},{2,-11},{2,-10},{1,-8},{4,-6},{4,-2},{3,0},{2,11},{3,11},{3,12},{0,12},{0,2},
{0,12},{-3,12},{-3,11},{-2,11},{-3,0},{-4,-2},{-4,-6},{-1,-8},{-2,-10},{-2,-11},{-1,-12}}; 

//Na razie definijemy przyk��dowe kszta�ty zawsze, bo jest ich niewiele
static Wielobok WRomb(kRomb,sizeof(kRomb)/sizeof(kRomb[0]));
static Wielobok WDomek(kDomek,sizeof(kDomek)/sizeof(kDomek[0]));
static Wielobok WUfo(kUfo,sizeof(kUfo)/sizeof(kUfo[0]));
static Wielobok WLudzik0(kLudzik0,sizeof(kLudzik0)/sizeof(kLudzik0[0]));
static Wielobok WLudzik1(kLudzik1,sizeof(kLudzik1)/sizeof(kLudzik1[0]));

//Dost�p Biblioteka podstawowych kszta�t�w w przestrzeni nazw klasy Wielobok, 
//Zrobione jako statyczne metody bo to pozwala robi� potem r�ne implementacje 
//przechowywania tych wielobok�w, np. tworzy� je dopiero gdy b�d� potrzebne
//lub �ci�ga� z dysku. 
//TU "static" by�o w klasie i oznacza co innego bo odnosi si� do metody!
const Wielobok&  Wielobok::Romb() {return WRomb;}
const Wielobok&  Wielobok::Domek(){return WDomek;}
const Wielobok&  Wielobok::Ufo() {return WUfo;}
const Wielobok& Wielobok::Ludzik(int typ) 
{
      if(typ==1) return WLudzik1;
         else return WLudzik0;
}  
      

// W�A�CIWA IMPLEMENTACJA KLASY WIELOBOK
//////////////////////////////////////////////////////////
//Obliczenia istotnych w�a�ciwo�ci
const int MAXPOS= 2147483647; //INT32_MAX z stdint.h
const int MINPOS=(-2147483647 - 1);//INT32_MIN z stdint.h
int Wielobok::DajMinX() const
{
  int Val=MAXPOS;//Najwi�kszy 
  for(unsigned i=0;i<Ilobok;i++)
  if(Val>Punkty[i].x) //Je�li Val jest wi�kszy ni� X
     Val=Punkty[i].x; //to X zostaje nowym Val 
  return Val; //Najmniejszy X
}

int Wielobok::DajMaxX() const
{
  int Val=MINPOS;//Najmniejszy 
  for(unsigned i=0;i<Ilobok;i++)
  if(Val<Punkty[i].x) //Je�li Val jest mniejszy od X
     Val=Punkty[i].x; //To X zostaje nowym Val
  return Val; //Najwi�kszy X
}

int Wielobok::DajMinY() const
{
  int Val=MAXPOS;//Najwi�kszy 
  for(unsigned i=0;i<Ilobok;i++)
  if(Val>Punkty[i].y)
     Val=Punkty[i].y;
  return Val; //Najmniejszy Y        
}

int Wielobok::DajMaxY() const
{
  int Val=MINPOS;//Najmniejszy 
  for(unsigned i=0;i<Ilobok;i++)
  if(Val<Punkty[i].y) 
     Val=Punkty[i].y; 
  return Val; //Najwi�kszy Y
}

//Transformacje. Modyfikuj� list� punkt�w �eby by�o wygodniej
void Wielobok::Centruj()
//Zmienia wsp�rz�dne tak, �eby by�y wok� �rodka ci�ko�ci
{
  unsigned i;
  //Najpierw trzeba znale�� �rodek ci�ko�ci figury
  double x=0,y=0;
  for(i=0;i<Ilobok;i++)
  {
     x+=Punkty[i].x;
     y+=Punkty[i].y;        
  }   
  x/=double(Ilobok);
  y/=double(Ilobok);
  
  //A teraz mo�na przesun�� do �rodka
  for(i=0;i<Ilobok;i++)
  {
     Punkty[i].x-=x;
     Punkty[i].y-=y;        
  }         
}

void Wielobok::Skaluj(double x,double y)
//Zmienia wsp�rz�dne
{
    unsigned i;
    for(i=0;i<Ilobok;i++)
    {
     Punkty[i].x*=x;
     Punkty[i].y*=y;        
    }         
}

void Wielobok::OdbijWPionie()
//Zmienia w odbicie lustrzane pionowo
{
    unsigned i;
    for(i=0;i<Ilobok;i++)
    {
     Punkty[i].y=-Punkty[i].y;        
    }        
}

void Wielobok::OdbijWPoziomie()
//Zmienia w odbicie lustrzane poziomo
{
    unsigned i;
    for(i=0;i<Ilobok;i++)
    {
     Punkty[i].x=-Punkty[i].x;     
    } 
}    

void Wielobok::ObrocORad(double Radiany)
//Obraca o ile� radian�w
{
    unsigned i;
    double cosR=cos(Radiany);
    double sinR=sin(Radiany);
    for(i=0;i<Ilobok;i++)
    {
     double x = Punkty[i].x;
     double y = Punkty[i].y;
     Punkty[i].x = x*cosR-y*sinR;       
     Punkty[i].y = x*sinR+y*cosR;
    }
}
     

//Konstruktory - tworz� wieloboki na bazie wzorca, tablicy albo innego Wieloboku    
//Nie ma mo�liwo�ci stworzenia pustego "Wieloboku", cho� mo�e by� "zerowy"   
//Tylko to ju� na odpowiedzialno�� u�ytkownika klasy.  
Wielobok::Wielobok(const ssh_point Wzorek[],unsigned RozmiarWzorka)
{                                                       assert(RozmiarWzorka>2);
   Punkty=new ssh_point[RozmiarWzorka];                 assert(Punkty!=NULL);
   Ilobok=RozmiarWzorka;
   memcpy(Punkty,Wzorek,Ilobok*sizeof(ssh_point));           
}

Wielobok::Wielobok(const Wielobok& Wzorek)
//Konstruktor kopuj�cy
{
   Punkty=new ssh_point[Wzorek.Ilobok];                  assert(Punkty!=NULL);
   Ilobok=Wzorek.Ilobok;
   memcpy(Punkty,Wzorek.Punkty,Ilobok*sizeof(ssh_point));           
}

//Destruktor - bo trzeba zwolni� pomocnicz� tablice
Wielobok::~Wielobok()
{
  delete []Punkty;
}

//Rysuj() - rysuje gdzie� wielobok w zadanym kolorze. Nie modyfikuje stanu!
void Wielobok::Rysuj(int StartX,int StartY,ssh_color Color) const
//Rysowanie tej figury w miejscu i kolorze zdefiniowanym gdzie indziej 
//Zrobione jako osobna procedura bo mo�emy j� rozbudowa� o dodatkowe
//elementy nie b�d�ce sk��dowymi samego wiekok�ta
{        
   fill_poly(StartX,StartY,Punkty,Ilobok,Color);  
}

const ssh_point&  Wielobok::DajPunkt(unsigned Ktory) const
{
    return Punkty[Ktory];
}

         

