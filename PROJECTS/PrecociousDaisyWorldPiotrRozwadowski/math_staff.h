 	#define WIDTH 100
 	#define HEIGHT 50
    unsigned int SIZE_X = 640;  // szeroko�� okna
 	unsigned int SIZE_Y = 480;  // wysoko�� okna
 	const unsigned short RED = 50;
 	const unsigned short BLUE = 150;
 	const unsigned short GREEN = 210;
 	const unsigned short WHITE = 255;    
 	const unsigned short BLACK = 256;
 	const unsigned short GRAY = 400;
 	const unsigned short NON = 1;
 	const unsigned short BACKGROUND = 200;
 	const float STEFANO_BOLTZMAN = 0.00000005669; // sta�a Stefano-Bolzmana
 	static const double pii = 3.1415927;
 	const float ideal_temperature = 295.15; // 22.5 celcjusza
 	float global_temperature;         // temperatura �wiata w Kelwinach
 	float solar_luminosity = 1200;//Stala sloneczna [W/m2] - energia jaka dociera do powierzchni
    float density_gray = 0.25;           //g�sto�� pocz�tkowa szarych
 	float density_white = 0.1;          //g�sto�� pocz�tkowa bia�ych
 	float density_black = 0.1;          //g�sto�� pocz�tkowa czarnych
 	int death_rate = 10;                  // 1/death_rate to prawdopodobie�stwo �mierci kwiatka
 	float ALBEDO_GRAY = 0.5;                 //albedo szarego
 	float ALBEDO_BLACK = 0.25;               //albedo czarnego
 	float ALBEDO_WHITE = 0.75;               //albedo bia�ego
 	float ALBEDO_SURFACE = 0.37;             //albedo Ziemi
 	unsigned int DELAY = 200;    // op�nienie
 	bool flowers_painting = true;            // czy kwiaty maj� by� rysowane 
 	
    int colorFromTemperature(float temp)
 	{
        return (int)(temp/500*300+50);    
    }
    
    void increase_solar_luminosity()
 	{
         solar_luminosity++;
         if (solar_luminosity >= 2500) 
             solar_luminosity = 2500;       
    }

    void decrease_solar_luminosity()
 	{
         solar_luminosity--; 
         if (solar_luminosity <= 0)
             solar_luminosity = 0;     
    }

 	void increase_delay()
 	{
         DELAY = DELAY * 5; 
         if (DELAY >= 1500) 
             DELAY = 1000;       
    }

    void decrease_delay()
 	{
         DELAY = DELAY / 5; 
         if (DELAY <= 0) 
             DELAY = 1; 
    }