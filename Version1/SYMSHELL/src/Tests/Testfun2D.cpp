//PLIK SZABLONU ZADANIA:
//Poszuka� w poni�szym kodzie sekwencji TODO i wykona� uzupe�nienia zgodnie ze 
//znajduj�cymi si� w tych miejscach wskaz�wkami.
//Przetestowany kod wys�a� na maila:
//              x.x@uw.edu.pl 

////////////////////////////////////////////////////////////////////////////////
//			Mapa funkcji 2D - przykladowy program SYMSHELLA 
//------------------------------------------------------------------------------
// Bardzo prosty test grafiki, ale z odtwarzaniem ekranu i obslug� zdarzen.
// POD Dev-Cpp potrzebne s� dwie bibloteki:
// ".../Dev-Cpp/lib/libgdi32.a" oraz "...Dev-Cpp/lib/libcomdlg32.a"
////////////////////////////////////////////////////////////////////////////////

#include "symshell.h"
#include <stdio.h> //Wyj�cie na konsole a la j�zyk C - printf(....)
#include <math.h>

//Wyj�ciowy rozmiar "ekranu"
const int size=400;

//Do wizualizacji obs�ugi zdarze�
const int DELA=100;//Oczekiwanie w obr�bie p�tli
const char* CZEKAM="Klikaj! ";
//Ostatni "klik" myszy
int xmouse=10,ymouse=10; 

//Badany zakres parametr�w funkcji
double Xstart=0;//-3.14;
double Xend=62.8;
double Ystart=0;//-3.14;
double Yend=62.8; 
 
void replot()
//Rysuje co� na ekranie
{
  int xsize=screen_width();//Aktualne mog� by� r�ne od poczatkowych!
  int ysize=screen_height()-char_height('X'); 
  
  double Xstep=(Xend-Xstart)/xsize;
  double Ystep=(Yend-Ystart)/ysize;
  printf("%g %g\n",Xstep,Ystep); //sprawdzenie na konsoli, jak w C
  //cout<<Xstep<<' '<<Ystep<<endl;//Albo bardziej w stylu C++
  
  int x,y;
  double XX,YY;
  for(x=0,XX=Xstart;XX<Xend;x++,XX+=Xstep)
    for(y=0,YY=Ystart;YY<Yend;y++,YY+=Ystep)  
      {
	    //TU WSTAWIAMY WYRAZENIE DO ZMAPOWANIA:
        //double ZZ=XX*YY; //Np. zwyk�e mno�enie
        double ZZ=sin(sqrt(XX))*sin(pow(YY,0.5));//Albo co� ciekawszego
		
        //TODO - powinna by� osobna funkcja np double fxy(double x,double y);
        //Wtedy b�dzie j� mo�na bezpiecznie u�y� zar�wno w funkcji replot()
        //jak i w funkcji zapisz_do_pliku()
        
        int z=int(ZZ*256)%256; //Cala skala na zakres 0..1, nast�pne warto�ci si� powtarzaj�
        if(z<0) z=z+256;   //"Obrocenie" ujemnych
      //  printf("%d ",z); //sprawdzenie warto�ci wydrukiem na konsoli
        plot(x,y,z);
      }
  //Ostatnie polozenie kliku - bia�y krzy�yk   
  line(xmouse,ymouse-10,xmouse,ymouse+10,255);
  line(xmouse-10,ymouse,xmouse+10,ymouse,255);
}

void zapisz_do_pliku()
{
     //TODO - funkcja powinna zapisa� funkcje do pliku zamiast j� wyrysowaywa� na ekranie
     //Format - tabela liczb typu double rozdzielanych tabulacjami
     //Nazwa pliku na razie ustalona z g�ry
}

void read_mouse()
//Przykladowa procedura obslugi myszy
{ 
   int xpos,ypos,click;
   if(get_mouse_event(&xpos,&ypos,&click)!=-1)//Operator & - pobranie adresu
   { /*
      xmouse=xpos;ymouse=ypos;
      fill_circle(xmouse,ymouse,10,254);  //TODO - zamiana k�ka na krzy�yk
      //TODO - wydruk warto�ci funkcji w tym punkcie na konsole                                          
      //...
	  */
   }                      
}

int main(int argc,char* argv[])//Potrzebne s� paramatry wywo�ania programu
{
 fix_size(0);
 mouse_activity(1); // Czy mysz b�dzie obs�ugiwana   
 buffering_setup(1);// Nie b�dzie pamieta� w bitmapie zawartosc ekranu 
 shell_setup("WIZUALIZACJA FUNKCJI 2D",argc,argv);// Przygotowanie okna  

 init_plot(size,size,0,1);/* Otwarcie okna SIZExSIZE pixeli + 1 wiersz znakow za pikselami*/

 /* Teraz mo�na rysowa� i pisac w oknie */
 replot();
 flush_plot();	/* Obraz/bitmapa gotowa */

 bool kontynuuj=true;	//Zmienna steruj�ca zako�czeniem programu
 unsigned loop=0;
 while(kontynuuj) //P�TLA OBS�UGI ZDARZE�
    { 
      int pom; //NA ZNAK Z WEJ�CIE OKNA GRAFICZNEGO
      loop++; //Zliczanie nawrot�w
      if(input_ready()) //Czy jest zdarzenie do obs�ugi?
      {
       pom=get_char(); //Przeczytaj nades�any znak
       switch(pom)
       {
       case 'p': zapisz_do_pliku();break;//TODO - t� fukncje trzeba powy�ej wype�ni� tre�ci�
       //TODO case 'x': //pyta o startow� warto�� X
       //TODO case 'X': //pyta o ko�cow� warto�� X
       //TODO case 'y': //pyta o startow� warto�� Y
       //TODO case 'Y': //pyta o ko�cow� warto�� Y
       case '\r': replot();break;//Wymagane odrysowanie
       case '\b': read_mouse();break;//Jest zdarzenie myszy
       case EOF:  //Typowe zako�czenie
       case  27:  //ESC
       case 'q':  //Zako�czenie zdefiniowane przez programiste
       case 'Q': kontynuuj=false;break;             
       default:
            print(0,screen_height()-char_height('N'),"Nie wiem co znaczy %c [%d] ",pom,pom);
            printf("Nie wiem co znaczy %c [%d] ",pom,pom);
            flush_plot();	// Bitmapa gotowa   
            break;
        }
      }
      else //Akcja na wypadek braku zdarze� do obslugi
      {
       if(loop%2==0)
        printc(0,screen_height()-char_height('C'),128,255,CZEKAM);
       else
        printc(0,screen_height()-char_height('C'),255,128,CZEKAM);
    
       flush_plot();// gotowe  - i wlasciwie co chwila wywolywane 
       delay_ms(DELA);//Wymuszenie oczekiwania - �eby p�tla nie zjada�a ca�ego procesora  
      } 
    }
 	    
 printf("Wykonano %d obrotow petli.\nDo widzenia!\n",loop);
 close_plot();/* Zamykamy okno - po zabawie */
 printf("Do widzenia!\n");
 return 0;
}


