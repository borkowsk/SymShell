////////////////////////////////////////////////////////////////////////////////
//			Przykladowy program SYMSHELLA z odtwarzaniem ekranu
//------------------------------------------------------------------------------
// Bardzo prosty test grafiki z funkcj� odtwarzania ekranu. 
// POD Dev-Cpp potrzebne s� dwie bibloteki:
// ".../Dev-Cpp/lib/libgdi32.a" oraz "...Dev-Cpp/lib/libcomdlg32.a"
////////////////////////////////////////////////////////////////////////////////
#include "symshell.h"
#include <algorithm> //bo min
#include <stdio.h>

//Rozmiar "ekranu"
const int size=400;

//Do wizualizacji obs�ugi zdarze�
const int DELA=100;//Oczekiwanie w obr�bie p�tli
const char* CZEKAM="Klikaj! ";
 
//Ostatni "klik" myszy
int x=10,y=10; 


void read_mouse()
//Przykladowa procedura obslugi myszy
{ 
   int xpos,ypos,click;
   if(get_mouse_event(&xpos,&ypos,&click)!=-1)//Operator & - pobranie adresu
   {
      x=xpos;y=ypos;
      fill_circle(x,y,10,254);                                              
   }                      
}

void replot()
//Rysuje co� na ekranie
{
  for(unsigned c=1;c<std::min(screen_width(),screen_height());c++)   
  {
     circle(0,0,c,255);
     line(size,size,c,size-2*c,c%256);//Musi by� % (modulo) bo inaczej kolor mo�e przekroczy� zakres
     //delay_ms(10);
  }
  fill_circle(screen_width()/2,screen_height()/2,10,128);
  //Dodaj krzy�yk w miejscu ostatniego klikni�cia
  //... TODO
  line(x-10,y,x+10,y,250);
  line(x,y-10,x,y+10,253);
}

int main(int argc,char* argv[])//Potrzebne s� paramatry wywo�ania programu
{
 fix_size(1);       // Czy emulujemy ekran o sta�ych rozmiarach?
 mouse_activity(1); // Czy mysz b�dzie obs�ugiwana   
 buffering_setup(0);// Nie b�dzie pamieta� w bitmapie zawartosc ekranu 
 shell_setup("VERY SIMPLE TEST 2",argc,argv);// Przygotowanie okna  

 init_plot(size,size,1,1);/* Otwarcie okna SIZExSIZE pixeli + 1x1 znakow za pikselami*/

 /* Teraz mo�na rysowa� i pisac w oknie */
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
       printc(0,screen_height()-char_height('C'),128,255,CZEKAM);
       flush_plot();// gotowe   
       delay_ms(DELA);//Wymuszenie oczekiwania - �eby p�tla nie zjada�a ca�ego procesora  
      } 
    }
 	    
 printf("Wykonano %d obrotow petli.\nDo widzenia!\n",loop);
 close_plot();/* Zamykamy okno - po zabawie */
 printf("Do widzenia!\n");
 return 0;
}


