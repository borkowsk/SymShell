 	// RUCHOME LUDZIKI - program SYMSHELLA pokazuj�cy idee ruchu celowego
    /////////////////////////////////////////////////////////////////////////////////// 
 	// POD Dev-Cpp potrzebne s� dwie bibloteki:
 	// ".../Dev-Cpp/lib/libgdi32.a" 
 	// oraz "...Dev-Cpp/lib/libcomdlg32.a"
 	
 	#include "symshell.h"
 	#include "Wieloboki.hpp"
 	#include <iostream>
 	#include <cstdlib>
 	#include <cmath>
 	using namespace std;
 	
 	const int size=500; //Bok obszaru roboczego
 	const int iluich=100; //ILU JEST AGENT�W!
 	const int RAND_START=25;//A mo�na np. time(NULL) - wtedy "bardziej losowo"
    
    class Agent
    {
      Wielobok  Ksztalt;
      double Xpoz;
      double Ypoz;
      unsigned Labilnosc; //Im wieksza wartosci tym cz�sciej zmienia cel
      ssh_color Kolor;
      
      //Parametry celu do kt�rego zmierza
      double Xcel;
      double Ycel;
      public:
      //Czytanie atrybut�w
      double X(){return Xpoz;}
      double Y(){return Ypoz;}   
      //Obliczanie ogranicze� prostok�ta opisanego na wielok�cie   
      double maxX(){ return Xpoz+Ksztalt.DajMaxX();}
      double minX(){ return Xpoz+Ksztalt.DajMinX();}
      double maxY(){ return Ypoz+Ksztalt.DajMaxY();}
      double minY(){ return Ypoz+Ksztalt.DajMinY();} 
      //Sprawdzanie kolizji z innym agentem
      friend //nie powinno by� metod� agenta, bo jest symetryczne, st�d "friend"
      bool Kolizja(Agent& A,Agent& B);
      //Zmiany atrybut�w
      void UstawKolor(ssh_color nKolor){ Kolor=nKolor%256; } //Zabezpieczona zmiana kolor�w
      void PrzesunDo(double NowyX,double NowyY){Xpoz=NowyX;Ypoz=NowyY;}//"R�czne" przesuwanie agenta w inne miejsce
      
      //Konstruktor  bezparametrowy - konieczny je�li maj� by� w tablicy     
      Agent():Ksztalt( Wielobok::Ludzik(0) )//,Xpoz(0),Ypoz(0)
             {
                Xpoz=rand()%size;
                Ypoz=rand()%size;
                Xcel=rand()%size;
                Ycel=rand()%size;
                Labilnosc=rand()%5;
                Kolor=50+Labilnosc*40+rand()%20;//Pierwsze 50 kolor�w jest raczej ciemne
                Ksztalt.Centruj();//�eby �rodek by� w �rodku :-)
                Ksztalt.Skaluj(1,1.5);//I wielko�� jaka� rozs�dna
             }
      //Destruktor - na razie nic jawnie nie robi
      //ale na wszelki wypadek wirtualny
      virtual ~Agent()
              {}     
      //Do wizualizacji        
      void Rysuj()
           {Ksztalt(Xpoz,Ypoz,Kolor);}    
      //Do symulacji     
      void ZrobKrok();     
    };
    
    Agent* Agenci;//"KOLEKCJA" AGENT�W - NIE B�D� ZNIKA� TO MO�E BY� TE� ZWYK�A TABLICA 
    
    bool Kolizja(Agent& P,Agent& Q)
   // czy prostok�ty AABB si� przecinaj�
   {//Sprawdzenie czy przecinanie jest wykluczone
    if( P.minX() > Q.maxX() || P.maxX() < Q.minX()
     || P.minY() > Q.maxY() || P.maxY() < Q.minY() ) 
        return false; //Gdy prawd� jest �e si� NIE przecinaj�
    else
    {
      //cout<<"Kolizja:"<<endl;
      //cout<<P.minX()<<' '<<P.maxX()<<" vs "<< Q.minX() <<' '<<Q.maxY()<<endl;
      //cout<<P.minY()<<' '<<P.maxY()<<" vs "<< Q.minY() <<' '<<Q.maxY()<<endl; 
      return true; //W przeciwnym przypadku
    }
   }
    
    void inicjalizacja()
    //U�o�enie pocz�tkowe i inne operacje konieczne do zrobienia na pocz�tku
    {
       srand(RAND_START); //Inicjalizacja generatora      
       Agenci=new Agent[iluich]; //Inicjalizacja agent�w - wywo�uj� si� konstruktory   
       
       for(unsigned i=0;i<iluich;i++) //Sprawdzamy kolizje i-tego agenta i poprawiamy do skutku
       {                                           cout<<i<<": ";
         bool OK; //Deklarowane tu bo musi by� widoczne poza p�tl� do{}while()                                        
         do
         { 
          OK=true; //Domy�lnie nie ma kolizji, chyba �e wykryjemy dalej
          for(unsigned j=0;j<i;j++) //Tylko z tymi "po spodem" (wcze�niejszymi)
            if( Kolizja(Agenci[i],Agenci[j]) )
            {                                       cout<<"kz:"<<j<<' ';         
              OK=false; //Wykryto kolizje
              Agenci[i].PrzesunDo( rand()%size, rand()%size ); //Przesuwamy w losow� pozycje
              break;  //Jedna kolizja wystarczy �eby by�o nie-OK i sprawdzanie od nowa
            }  
          }while(!OK);  /*A� si� uda */              cout<<endl;
       }
    }     
    
    void Agent::ZrobKrok()
    {   
       //Zabezpieczenie przed wy�a�eniem poza "pole walki" - niepe�ne...
       //Na razie wiemy �e cel nigdy nie losuje si� poza ekranem  
       if(Xpoz<Xcel) Xpoz++;
          else Xpoz--;
       if(Ypoz<Ycel) Ypoz++;
          else Ypoz--;    
       //Czasami cel si� zmienia losowo. 
       if(rand()%(size/(1+Labilnosc))==0) //TODO - prawdopodobie�stwo zmiany mo�e by� cech� indywidualn�!
       {
          Xcel=rand()%size;
          Ycel=rand()%size;               
       }  
    }
    
    void krok()
    //Zmiany u�o�enia zwi�zane z r�nymi ruchami
    {       
       //Przesuwanie
       for(unsigned i=0;i<iluich;i++)
       {
        double PoprzedniX=Agenci[i].X();
        double PoprzedniY=Agenci[i].Y();
        Agenci[i].ZrobKrok();   
        //Sprawdzanie kolizji agenta z innymi
        for(unsigned j=0;j<iluich;j++)
        {
          if(i!=j && Kolizja(Agenci[i],Agenci[j]) )
          {
           Agenci[i].PrzesunDo(PoprzedniX,PoprzedniY);//Wycofaj ruch
           break;//Przerwanie p�tli sprawdzania, gdy ruch wycofany
          }
        }
       }                                     
    }
   
    void replot()
    //Procedura odrysowujaca aktualny stan
 	{
      clear_screen();//Czyszczenie ca�ego ekranu na wszelki wypadek
      //TODO:UPORZ�DKOWANIE WG. Y-kow, ZEBY SI� DOBRZE ZAS�ANIA�Y 
      // qsort(...), 
      /* void qsort(
              void *base,
              size_t num,
              size_t width,
              int (__cdecl *compare )(const void *, const void *) 
       );*/
      //ale bezpo�rednio u�yty na tablicy Agenci by�by skrajnie nieefektywny,  
      //bo by musia� przestawia� w pami�ci ca�ych agent�w!
      //Trzeba zmieni� arganizacj� �wiata na tablice wska�nik�w, albo zrobi�
      //tak� tablic� wska�nik�w lokalnie
      
      //TERAZ MO�NA JU� RYSOWA�
      for(unsigned i=0;i<iluich;i++)
        Agenci[i].Rysuj();
        
 	  flush_plot();	// Bitmapa gotowa 
 	}

    void statystyka()
    {
         //...NA POTEM
    }

 	//Zmienne do obs�ugi zdarze� i dalej funkcje kt�re to robi�, w tym main()
 	const int WAIT=10; //Co ile nawr�t�w p�li zmienia� kolor
    const int DELA=100/WAIT;//Oczekiwanie w obr�bie p�tli
 	const char* CZEKAM="..."; 
 	unsigned long loop=0;

 	void read_mouse() //Procedura obslugi "kliku" myszy
 	{ 
       int xpos,ypos,click;
       if(get_mouse_event(&xpos,&ypos,&click)!=-1)//Operator & - pobranie adresu
	   {
          //.....x=xpos;
          //.....y=ypos;
          replot();                                                   
       }                      
 	}

 	int main(int argc,char* argv[])//Potrzebne s� paramatry wywo�ania programu
 	{
       mouse_activity(1);       //Czy mysz b�dzie obs�ugiwana   
       buffering_setup(1);      //Czy pamietac w bitmapie zawartosc ekranu TAK
 	   fix_size(1);	            //Czy symulowa� niezmiennosci rozmiarow okna TAK
 	   shell_setup("SYMULACJA AGENTOWA",argc,argv);//Ustalenie tytu�u i konfiguracji z linii komend
 	   init_plot(size,size,0,1);// size x size pixeli + 1 wiersz znakow pod spodem
 	   
 	   inicjalizacja();         //Przygotowanie paramer�w
 	   
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
      statystyka(); //Obliczanie chwilowych statystyk
      replot();  //Rysowanie   
      if(loop%WAIT<WAIT/2)
         printc(0,screen_height()-char_height('C'),128,255,"%u %s",loop,CZEKAM);
      else
        printc(0,screen_height()-char_height('C'),255,28,"%u %s",loop,CZEKAM);       
 	  flush_plot();// Bitmapa po modyfikacji ju� gotowa   
 	  delay_ms(DELA);//Wymuszenie oczekiwania - �eby p�tla nie zjada�a ca�ego procesora  
                 //I �eby si� wszystko nie dzia�o zbyt szybko!
 	  } 
 	}

 	cout<<"Dealokacja tablicy agent�w"<<endl;
 	delete []Agenci; //Taka dziwna sk�adnia, bo wywo�uje destruktory!

 	cout<<"Wykonano "<<loop<<" obrotow petli.\nDo widzenia!\n"<<endl;
 	close_plot(); //Po zabawie - zamykam okno
 	return 0;
 	}
