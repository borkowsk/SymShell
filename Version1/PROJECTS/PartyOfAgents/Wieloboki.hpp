//  Klasa wielobok i biblioteka r�nych wielobok�w
//  Przyk�ad definiowania dosy� prostej klasy
//        ale maj�cej przeci��one operatory [] i ()
//////////////////////////////////////////////////////////////
#include "symshell.h" //Potrzebne typy z symshell'a

//Definicja klasy Wielobok - niemal bez �adnej implementacji metod
class   Wielobok
//////////////////////////////////
{
private: //Pola prywatne - nikt nie ma bezpo�redniego dost�pu    
//Takie pola b�d� w ka�dym obiekcie tej klasy: 
ssh_point* Punkty; //Wska�nik do listy punkt�w      
unsigned   Ilobok;//"Ilobok"? Czyli ile jest punkt�w na li�cie  

public: //Interface - metody widoczne dla u�ytkownik�w klasy

//Metody dzia�aj�ce te� dla Wielobok�w sta�ych (const)  
unsigned IlePunktow() const {return Ilobok;} //To mo�e by� "inline" - szybkie   
const ssh_point& DajPunkt(unsigned Ktory) const;//A to lepierj nie tu, bo "zaciemni" kod
        //Ta metoda zwraca "sta�� referencj�" do punktu, co pozwala na czytanie
        //ale nie pozwala modyfikowa� tego punktu
        
int DajMinX() const;
int DajMaxX() const;
int DajMinY() const;
int DajMaxY() const;

const ssh_point& operator [] (unsigned Ktory) const //Wersja operatorowa!!!!
                 {return DajPunkt(Ktory);}        

//Rysuj() - rysuje gdzie� wielobok w zadanym kolorze. Nie modyfikuje stanu!
void Rysuj(int x,int y,ssh_color Color) const;  

void operator () (int x,int y,ssh_color Color) const //Wersja operatorowa!!!!
                 { Rysuj(x,y,Color);}

//Konstruktory - tworz� obiekty klasy Wielobok 
//W tym wypadku tylko na bazie wzorc�w - tablic albo innych Wielobok�w     
//Nie ma mo�liwo�ci stworzenia pustego "Wieloboku", 
//cho� na upartego mo�e by� "zerowy"  
Wielobok(const ssh_point Wzorek[],unsigned RozmiarWzorku);
Wielobok(const Wielobok& Wzorek); //tzw. konstruktor kopuj�cy, kt�rego parametrem 
               //jest odniesienie - tzw. "sta�a referencja" do obiektu tego samego
               //typu. Jego zawarto�� musi by� starannie przekopiowana

//Destruktor - bo trzeba zwolni� pomocnicz� tablice.
//"virtual" - bo tak jest bezpieczniej.
//Co to znaczy b�dzie innym razem :-) 
virtual ~Wielobok();       

//Transformacje. Modyfikuj� list� punkt�w �eby by�o wygodniej
void OdbijWPionie();//Zmienia w odbicie lustrzane pionowo
void OdbijWPoziomie();//Zmienia w odbicie lustrzane poziomo
void ObrocORad(double Radiany);//Obraca o ile� radian�w
void Skaluj(double x,double y);//Zmienia wsp�rz�dne
void Centruj();//Zmienia wsp�rz�dne tak, �eby by�y wok� �rodka ci�ko�ci
     //Uwaga - trzy ostatnie s� "niszcz�ce", bo punkty s� pami�tane 
     //na liczbach ca�kowitych a wyniki nie musz� takie by�!
              
//Biblioteka podstawowych kszta�t�w w przestrzeni nazw klasy Wielobok, 
//Zrobione jako statyczne metody bo to pozwala robi� potem r�ne implementacje 
//przechowywania tych wielobok�w, np. tworzy� je dopiero gdy b�d� potrzebne
//lub �ci�ga� z dysku. 
//Zwracaj� "sta�e referencje" czyli mo�na czyta�, rysowa�, ale nie zmienia�. 
//"static" w odniesieniu do metody oznacza, �e jest to sk�adowa klasy jako takiej
// a nie ka�dego obiektu tej klasy z osobna. Do wywo�ania obiekt nie jest potrzebny!
static const Wielobok& Romb();
static const Wielobok& Domek();
static const Wielobok& Ufo();     
static const Wielobok& Ludzik(int typ); 
};


