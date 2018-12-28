////////////////////////////////////////////////////////////////////////////////
//
//      �wiat stokrotek                                                                        
//                                  Piotr Rozwadowski                                                            
//
////////////////////////////////////////////////////////////////////////////////

	#include "symshell.h"
 	#include "Wieloboki.hpp"
 	#include "math_staff.h"
 	#include <iostream>
 	#include <cstdlib>
 	#include <cstdio>
 	#include <cmath>
 	#include <cassert>
 	using namespace std;
    // POTRZEBNE WIELOBOKI 	
 	Wielobok plant(Wielobok::Plant());
 	Wielobok surface(Wielobok::Surface());
 	Wielobok center_plant(Wielobok::Center_plant());
 	Wielobok sun(Wielobok::Sun());
 	Wielobok sun_center(Wielobok::Sun_center()); 	

 	// KLASA POJEDYNCZEGO POLA	
 	class Field 
    {
         private:         
                  float temperature;
                  int color;
                                    
         public:
                  Field()
                  {
                  
                  }  
                  float getTemperature()
                  {
                       return temperature;      
                  }
                  void setTemperature(float t)
                  {
                      temperature = t;     
                  }
                  void setColor(int col)
                  {
                      color = col;               
                  }    
                  int getColor()
                  {   
                      return color;    
                  }  
                  void paint(int i) // rysowanie kwiatka
                  {                       
                      if (color != NON && flowers_painting)
                      {
                          if (i%2==0)
                          {
                               plant.Rysuj(180+(i%WIDTH)*15,int(i/WIDTH)*18,color);
                               center_plant.Rysuj(180+(i%WIDTH)*15,int(i/WIDTH)*18,250);   
                          }   
                          else
                          {
                               plant.Rysuj(180+(i%WIDTH)*15,int(i/WIDTH)*18+8,color);
                               center_plant.Rysuj(180+(i%WIDTH)*15,int(i/WIDTH)*18+8,250);                          
                          }   
                      }
                  }
                  void beam() // o�wietlenie pola
                  {
                       float albedo;
                       if (color==GRAY) albedo = ALBEDO_GRAY;
                       else if (color==WHITE) albedo = ALBEDO_WHITE;
                       else if (color==BLACK) albedo = ALBEDO_BLACK;
                       else albedo = ALBEDO_SURFACE;
                       double temp = solar_luminosity * (1-albedo)  / STEFANO_BOLTZMAN*0.5; // z prawa Stefano-Bolzmana
                       temperature = pow(temp, 0.25);
                  }     
    };
    
    Field board[WIDTH*HEIGHT];
    Field newBoard[WIDTH*HEIGHT]; // dodatkowy board by nie zapisywa� i wczytywa� naraz z jednego 
 	
 	void initialization(int argc,char* argv[])
    {   
         if(argc == 13) //argumenty z wiersza polece�
         {
             solar_luminosity = atof(argv[1]);       //irradiancja pocz�tkowa
             density_gray = atof(argv[2]);           //g�sto�� pocz�tkowa szarych
 	         density_white = atof(argv[3]);          //g�sto�� pocz�tkowa bia�ych
 	         density_black = atof(argv[4]);          //g�sto�� pocz�tkowa czarnych
             while(density_gray + density_white + density_black > 1) // na wszelki wypadek gdyby kto� chcia� mie� wi�cej kwiatk�w ni� p�l 
             {
                  density_gray = density_gray / 2;  
                  density_white = density_white / 2;
                  density_black = density_black / 2;                   
             }
             death_rate = atoi(argv[5]);                  //umieralno��             
             ALBEDO_GRAY = atof(argv[6]);                 //albedo szarego
 	         ALBEDO_BLACK = atof(argv[7]);               //albedo czarnego
 	         ALBEDO_WHITE = atof(argv[8]);               //albedo bia�ego
 	         ALBEDO_SURFACE = atof(argv[9]);             //albedo Ziemi
 	         if(ALBEDO_GRAY > 1)
 	             ALBEDO_GRAY = 1;
 	         else if (ALBEDO_GRAY < 0)
                 ALBEDO_GRAY = 1;
 	         if(ALBEDO_WHITE > 1)
 	             ALBEDO_WHITE = 1;
 	         else if (ALBEDO_WHITE < 0)
                 ALBEDO_WHITE = 1;
 	         if(ALBEDO_BLACK > 1)
 	             ALBEDO_BLACK = 1;
 	         else if (ALBEDO_BLACK < 0)
                 ALBEDO_BLACK = 1;                                   
 	         DELAY = atoi(argv[10]); 
         }
         srand(time(NULL));    // uruchamiamy generator liczb pseudolosowych                                                          
         for(int i=0; i<WIDTH*HEIGHT; i++)  // tworzenie pustych p�l
              board[i].setColor(NON);
         int i = 0, temp; // zmienne pomocnicze
         /// Tworzenie kwiat�w
         while(i < WIDTH*HEIGHT*density_white)
         {
              temp = rand()%(WIDTH*HEIGHT);
              if (board[temp].getColor() == NON)
              {
                   board[temp].setColor(WHITE);
                   i++;
              }
         }
         i = 0;
         while(i < WIDTH*HEIGHT*density_black)
         {
              temp = rand()%(WIDTH*HEIGHT);
              if (board[temp].getColor() == NON)
              {
                   board[temp].setColor(BLACK);
                   i++;
              }
         }
         i = 0;
         while(i<WIDTH*HEIGHT*density_gray)
         {
              temp = rand()%(WIDTH*HEIGHT);
              if (board[temp].getColor() == NON)
              {
                   board[temp].setColor(GRAY);
                   i++;
              }
         }  
     }    
     
     int neighbor(int i, int n) // wyb�r s�siada - troch� dziwny, ale przecie� siatka jest z sze�ciobok�w foremnych 
     {
          switch (i)
          {
               case  1:  return n-1;
                         break;
               case  2:  return n+1;
                         break;
               case  3:  return n-WIDTH;
                         break;
               case  4:  return n+WIDTH;
                         break;
               case  5: 
                         if(n%WIDTH==0 || n%WIDTH==WIDTH-1)
                              return n+WIDTH-1;
                         if(n%2==0)
                              return n-WIDTH-1;
                         else
                              return n+WIDTH-1;
                         break;
               case  6:                                                                           
                         if(n%30==0 || n%30==WIDTH-1)
                              return n-WIDTH+1;
                         if(n%2==0)
                              return n-WIDTH+1;
                         else
                              return n+WIDTH+1;
                         break;
          }
    }    
    
  	double normal(const double &mean, const double &std) // losuje warto�� z rozk�adu normalnego o �redniej mean i dyspersji std
    {
         static const double r_max = RAND_MAX+1;
         return std*sqrt(-2*log((rand()+1)/r_max))*sin(2*pii*rand()/r_max)+mean;
    }
     
    bool is_neighbor(int i, int c) // czy istnieje jaki� s�siad o takim samym kolorze
    {
         for (int j = 1; j<=6;j++)
         if(c == board[neighbor(j,i)].getColor())
              return true;                                      
         return false;     
    }

    void birth(int c1,int c2,int c3, int i)
    {   
         float diff = abs(ideal_temperature - global_temperature); //r�nica mi�dzy idealn� temperatur� a panuj�c�
         double n = abs(normal(ideal_temperature, 5.5) - ideal_temperature); 
         if (diff < n && is_neighbor(i,c1) && board[i].getColor()==NON)  
              board[i].setColor(c1);
         n = abs(normal(ideal_temperature,5.5) - ideal_temperature);   
         if (diff<n && is_neighbor(i,c2) && board[i].getColor()==NON)  
              board[i].setColor(c2);
         n = abs(normal(ideal_temperature,5.5) - ideal_temperature);   
         if (diff < n && is_neighbor(i,c3) && board[i].getColor()==NON) 
              board[i].setColor(c3);
    }
          
               
    void step()
    {   
           
         //tworzenie nowego stanu newBoard              
         for(int i=0; i<WIDTH*HEIGHT;i++)
              newBoard[i] = board[i]; 
         
         // o�wietlenie p�l
         for(int i =0; i<WIDTH*HEIGHT;i++)
         {
              newBoard[i].beam();
              board[i].beam();
         }
          // ciep�o p�l oddzia�uje z otoczeniem
         for(int i = 0; i<WIDTH*HEIGHT; i++)
         {
              float average = 0;
              int counter = 0;
              for(int j=1; j<=6; j++)
              {
                   int n = neighbor(j,i); 
                   if (n >= 0 && n < WIDTH*HEIGHT)     
                   {
                       average = average + board[n].getTemperature();
                       counter++;
                   }
              }
              average = average/counter;
              newBoard[i].setTemperature((average+board[i].getTemperature())/2);
         }                     
          // przepisanie rezultatu spowrotem na board          
         for(int i=0; i<WIDTH*HEIGHT;i++)
              board[i] = newBoard[i];
                  
         // narodziny i �mier� kwiatk�w                  
         for(int i=0; i <WIDTH*HEIGHT;i++)
         {
              int swt = rand()%6;   
              switch(swt)           //losowanie kolejno�ci po to by �aden kolor nie mia� przewagi
              {
                   case 0: birth(BLACK,WHITE,GRAY,i);
                   case 1: birth(BLACK,GRAY,WHITE,i);
                   case 2: birth(WHITE,BLACK,GRAY,i);
                   case 3: birth(WHITE,GRAY,BLACK,i);
                   case 4: birth(GRAY,WHITE,BLACK,i);
                   case 5: birth(GRAY,BLACK,WHITE,i);            
              }; 

              // �mier� kwiatka o prawdopodobie�stwie 1/death_rate   
              if (board[i].getColor()!=NON && rand()%death_rate==0)  
                   board[i].setColor(NON);
         }                 
         flush_plot();                              
    }

    float count(int color) // zlicza ilo�� p�l z danymi kwiatkami
 	{
         float result = 0;
         for(int i = 0; i<WIDTH*HEIGHT;i++)
              if(color == board[i].getColor())
                  result++; 
         return result;    
    }
    
    void replot() // Rysowanie okna
 	{
         clear_screen();
         //S�oneczko 
         sun.Rysuj(-5,-5,50); 
         sun.Rysuj(5,5,50); 
         sun.Rysuj(0,0,250);
         sun_center.Rysuj(37,43,55);
         //pola
         for(int i = 0; i<WIDTH*HEIGHT; i++)
         {
             if (i%2==0) // if bo mamy sze�ciok�ty :) 
                 surface.Rysuj(180+(i%WIDTH)*15,int(i/WIDTH)*18,colorFromTemperature(board[i].getTemperature()));
             else
                 surface.Rysuj(180+(i%WIDTH)*15,int(i/WIDTH)*18+8,colorFromTemperature(board[i].getTemperature()));
             board[i].paint(i);      
         }
         //panel z lewej strony
         printc(0,200,128,200,"Irradiancja: %g W/m^2",solar_luminosity,"");
         printc(0,220,128,200,"Temperatura: %g C",global_temperature-273.15,"");
         for(int i=197; i < global_temperature; i++) //pasek temperatury
         {
              if (global_temperature < 278)
                  line(i-197,240,i-197,260,BLUE);
              else if (global_temperature > 313)
                  line(i-197,240,i-197,260,RED);
              else
                  line(i-197,240,i-197,260,GREEN);
         }
         for(int i=360; i > 360-count(WHITE)/780*100; i--) //kolumna bia�a
              line(20,i,50,i,WHITE);
         for(int i=360; i > 360-count(GRAY)/780*100; i--)  //kolumna szara
              line(70,i,100,i,GRAY+20);
         for(int i=360; i > 360-count(BLACK)/780*100; i--) //kolumna czarna
              line(120,i,150,i,BLACK);         
         printc(10,360,WHITE,BACKGROUND,"%2.2f %%",count(WHITE)/780*100,"");
         printc(63,360,GRAY+30,BACKGROUND,"%2.2f %%",count(GRAY)/780*100,"");
         printc(117,360,BLACK,BACKGROUND,"%2.2f %%",count(BLACK)/780*100,"");
 	}

    void stats()
    {    
         float sum = 0; // obliczanie temperatury �wiata
         for (int i = 0; i<WIDTH*HEIGHT;i++)
              sum = sum + board[i].getTemperature();
         global_temperature = sum / WIDTH/HEIGHT;        
    }
    
    const int WAIT=10; //Co ile nawr�t�w p�li zmienia� kolor
    const int DELA=10/WAIT;//Oczekiwanie w obr�bie p�tli
 	const char* CZEKAM=" - "; 
 	unsigned long loop=0;

 	void read_mouse() //Procedura obslugi "kliku" myszy
 	{ 
       int xpos,ypos,click;
       if(get_mouse_event(&xpos,&ypos,&click)!=-1)
	   {
          replot();                                                   
       }                      
 	}
 	

    
 	int main(int argc,char* argv[])
 	{        
       mouse_activity(0);           //Ob�suga myszy   
       buffering_setup(1);          //Zawarto�� ekranu w bitmapie
       set_background(BACKGROUND);  //Kolor t�a
 	   fix_size(1);	                //Symulacja niezmiennosci rozmiarow okna 
 	   shell_setup("�wiat stokrotek",argc,argv);//Tytu� i konfiguracji z linii komend
 	   if (WIDTH>30) SIZE_X = SIZE_X + 15*(WIDTH-30);
 	   if (HEIGHT>26) SIZE_Y = SIZE_Y + 20*(HEIGHT-26);
 	   init_plot(SIZE_X,SIZE_Y,0,1); // size x size pixeli + 1 wiersz znakow pod spodem
 	   
 	   initialization(argc,argv);         //inicjalizacja
 	   
       bool continue_program = true;	//Kontynuacja programu
       while(continue_program)        //P�TLA OBS�UGI ZDARZE�
 	   { 
 	       int order;
 	       loop++; //Zliczanie nawrot�w
           if(input_ready()) //Czy jest co� do obs�ugi?
 	       {
 	            order=get_char(); //Przeczytaj nades�any znak
 	            switch(order)
 	            {
 	                 case '\r':                //Wymagane odrysowanie
                                replot();
                                break;
 	                 case '\b':                //Zdarzenie myszy
                                read_mouse();
                                break;
 	                 case  43:                 // + zwi�kszenie irradiancji
                                increase_solar_luminosity();                
                                break; 
 	                 case  45:                // - zmniejszenie irradiancji       
                                decrease_solar_luminosity(); 
                                break; 
 	                 case  44:                // , zmniejszenie op�nienia                  
                                decrease_delay();
                                break; 
 	                 case  46:                // . zwi�kszenie op�nienia
                                increase_delay();
                                break;                                          
 	                 case 'c':                // c zmiana wy�wietlania kwiat�w                                              
                                flowers_painting = !flowers_painting; 
                                break;
 	                 case EOF:  //Typowe zako�czenie
 	                 case  27:  //ESC
 	                 case 'q':  //Zako�czenie zdefiniowane przez programiste
 	                 case 'Q':  
                                continue_program = false;
                                break;             
 	                 default:
                                print(0,screen_height()-char_height('N'),"Nie wiem co znaczy %c [%d] ",order,order);
 	                            flush_plot();	// Bitmapa gotowa   
 	                            break;
                }
                cout<<"Zareagowano na kod nr "<<int(order)<<" w cyklu "<<loop<<endl;   
 	       }
 	       else //Brak zdarze�
 	       {
                stats();   //Statystyki
                step();    //Krok
                replot();  //Rysowanie   
                if(loop%WAIT < WAIT / 2)
                     printc(0,screen_height()-char_height('C'),128,BACKGROUND,"%u %s cykli",loop,CZEKAM);
                else
                     printc(0,screen_height()-char_height('C'),255,BACKGROUND,"%u %s cykli.",loop,CZEKAM);       
 	            flush_plot();  
 	            delay_ms(DELAY); //Wymuszenie oczekiwania
 	       } 
 	  }

 	  cout<<"Czyszczenie pami�ci"<<endl;
      delete []board;
      delete []newBoard;
 	  cout<<"Wykonano "<<loop<<" obrotow petli.\nDo widzenia!\n"<<endl;
 	  close_plot(); 
 	  return 0;
 	}