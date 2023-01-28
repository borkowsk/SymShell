//SYMULACJA KONFLIKTOW BOCA 2005 - UZUPELNIONY 10-11.2005, 9-2010, 02-2014
////////////////////////////////////////////////////////////////////////////////
//1.1 Dodano obsluge wczytywania sieci z pliku
//1.2 Dodano obsluge parametr�w wywolania 
//      i symetryczne traktowanie polaczen wczytanych z pliku
//1.21 Poprawiono obsluge parametr�w - blad dla string�w
//     Dodano parametry kontroli wydruku oraz Min-Max dla skali stan�w
//1.22 Updajte do poprawioenej wersji bibliotek SYMSHELL i WBRTM

const char* SYMULATION_NAME="CONFLICTS 1.22 BOCA/WARSZAWA: "__DATE__;
const char* SCREENDUMPNAME="CONFLICTS_v1.22_";
unsigned lang_selector=1;

const char* LogName="conf.log";  //Nazwa tego logu
const char* MetaExpFileName="metaconf.out";//Nazwa logu meta-eksperymentu
const char* DefaultNetFileName="conflicts1.net"; //Nazwa pliku z definicj� sieci

//ROZMIARY OKNA
unsigned SWIDTH=750;//1200;
unsigned SHEIGHT=552;//1024;

float  DIAM_SIZE=0.01f;
unsigned  ARROWS_DIV=7;//Dzielnik skracajacy strzalki. Jak 0 to maja stala dlugosc
double FixMinState=0;
double FixMaxState=0;

//Ogolne parametry modelowania
int MultiExp=1; //Ile zrobic powtorzen?
unsigned iMaxIterations=10000;    //Ile najwiecej krokow
unsigned iMaxIterRep=1000;
unsigned iViewRatio=1;           //Jak czesto wizualizowac
unsigned iLogRatio=1;            //Jak czesto zapisywac do logu

//Parametry wlasciwego modelu
int     def_mode=1; //0,1,2,3,4 --> 2 i 4 oznacza wczytanie sieci z pliku
unsigned def_num_of_nodes=25; //Ile jest wezlow gdy siec nie jest z pliku
//Stany poczatkowe i zmiany
double def_m_init_st=0.005;  //Sredni stan poczatkowy
double def_r_init_st=0.000001;   //Odchylenie od sredniego stanu poczatkowego
double def_m_of_noise=-0.1;  //Sredni poziom szumu/sygnalu dodawanego do wezlow
double def_r_of_noise=0.1;//.01;  //Odchylenie od sred. szumu/sygnalu dodawanego do wezlow
//Parametry zakresu zmian dla multi-experymentow
double defm_start_mean_state=-1.1;
double defm_end_mean_state=1.1;
double defm_start_noise_mean=-0.5;
double defm_end_noise_mean=0.5;
//Te na razie zafiksowane:
double def_m_of_weight=0.1; //Srednia waga krawedzi
double def_r_of_weight=0.0001; //Odchylenie od sred. wagi krawedzi
double def_asymmetry=1;   //Asymetria przekazywania komunikatow dodatnich wzgledem ujemnych    

//'Nazwy niektorych lufcik�w
const char* HISTofSTATES="HISTORY OF THE STATE";
const char* HISTofACCT="HISTORY OF THE ACTIVITY";
const char* META_GRAPH="INPUT vs. MEAN STATE";

#include <cassert>
#include <ctype.h>
#include <iostream>
using namespace std;
#include <cmath>

#ifndef M_PI
#define M_PI       3.14159265358979323846
#endif

#include "INCLUDE/platform.hpp" //????

//#define POLISH_VERSION
#include "INCLUDE/OptParam.hpp"
#include "INCLUDE/wb_ptr.hpp"
#include "INCLUDE/wb_rand.hpp"

#include "SYMSHELL/simpsour.hpp"
#include "SYMSHELL/funcsour.hpp"
#include "SYMSHELL/statsour.hpp"
#include "SYMSHELL/fifosour.hpp"
#include "SYMSHELL/sourmngr.hpp"
#include "SYMSHELL/gadgets.hpp"
#include "SYMSHELL/graphs.hpp"
#include "SYMSHELL/areamngr.hpp"
#include "SYMSHELL/mainmngr.hpp"
#include "SYMSHELL/world.hpp"

OptionalParameterBase* Parameters[]={ //sizeof(Parameters)/sizeof(Parameters[])
new OptionalParameter<const char*>(LogName,"*.log","e.g confl1.log","LOGNAME","Single experiment outputfile name"),
new OptionalParameter<const char*>(MetaExpFileName,"*.out","e.g mult1.out","MOUTNAME","Multiple experiment outputfile name"),
new OptionalParameter<const char*>(DefaultNetFileName,"*.net","e.g mynet.net","NETNAME","Network definition input file name"),
new OptionalParameter<int>(def_mode,0,3,"IMODE","Network initialisation mode: 0,1,3-random, 2-from definition file"),

new OptionalParameter<unsigned>(iMaxIterations,10,1000000,"MAXITER","Limit for number of simulation steps"),
new OptionalParameter<unsigned>(def_num_of_nodes,2,10000,"NODEN","How many nodes in random networks"),
new OptionalParameter<double>(def_m_of_weight,-1,1,"MEWEIGHT","Mean edge weight"),
new OptionalParameter<double>(def_r_of_weight,0,1,"REWEIGHT","Diference from mean edge weight"),
new OptionalParameter<double>(def_m_init_st,-1,1,"MISTATE","Mean initial state"),
new OptionalParameter<double>(def_r_init_st,0,1,"RISTATE","Diference from mean init state"),
new OptionalParameter<double>(def_m_of_noise,-1,1,"MNOISE","Mean noise/signal for nodes"),
new OptionalParameter<double>(def_r_of_noise,0,1,"RNOISE","Diference from mean noise/signal"),
new OptionalParameter<double>(def_asymmetry,0,100,"ASSYM","Asymetry in positive/negative communication"),

new OptionalParameter<int>(MultiExp,2,1000,"MULTI","Multiplication factor for multiple experiments"),
new OptionalParameter<double>(defm_start_mean_state,-2,2,"SMISTATE","Start value for mean state"),
new OptionalParameter<double>(defm_end_mean_state,-2,2,"EMISTATE","End value for mean state"),
new OptionalParameter<double>(defm_start_noise_mean,-2,2,"SMNOISE","Start value for mean noise/signal"),
new OptionalParameter<double>(defm_end_noise_mean,-2,2,"EMNOISE","End value for mean noise/signal"),

new OptionalParameter<unsigned>(iMaxIterRep,10,1000000,"MMAXSTEP","Limit for number of steps during multi experiments"),

new OptionalParameter<unsigned>(iViewRatio,1,  1000000,"VIEWR","simulation step to visualisation ratio"),
new OptionalParameter<unsigned>(iLogRatio,1,   1000000,"LOGR","simulation step to log writing ratio"), 
new OptionalParameter<unsigned>(SWIDTH,100,10000,"WINW","Initial width of program main window"),
new OptionalParameter<unsigned>(SHEIGHT,100,10000,"WINH","Initial height of program main window"),
new OptionalParameter<double>(FixMinState,-100,100,"FIXMINS","Min for fixed range of state in graphs"),
new OptionalParameter<double>(FixMaxState,-100,100,"FIXMAXS","Max for fixed range of state in graphs"),
new OptionalParameter<float>(DIAM_SIZE,0.0000000001,1,"DIAMS","Diamond size in the meta exp. visualisation"),
new OptionalParameter<unsigned>(ARROWS_DIV,1,100,"ARRDIV","Divider for arrows in the meta exp. visualisation")
};


double niby_gauss_max(double mean,double max,unsigned steps=6)//12
{
    double ret,A=0;             assert(mean<max);    
    double range=(max-mean);
    double min=mean-range;
    
    for(int i=steps;i>0;i--)
        A+=TheRandG.DRand(); 

    ret=A;                      assert(0.0<=ret && ret<=steps);
    ret/=steps;                 assert(0.0<=ret && ret<=1.0);
    ret*=range*2;               assert(0.0<=ret && ret<=2*range);
    ret+=min;                   assert(min<=ret && ret<=max);                  
    return ret;
}

//Swiat - GLOWNA KLASA SYMULACYJNA i JEJ PODJEDNOSTKI Agent & Connection
///////////////////////////////////////////////////////////////////////////
class Swiat:public world
////////////////////////
{
public:
    class Agent
	{
        double x,y,r;   //Wspolrzedne wizualizacyjne agenta
		double state;   //Stan agenta -1..1 (?)
        double pstate;  //Poprzedni stan agenta
        double delta;   //Ostatni wplyw od innych
        wb_pchar Name;  //Nazwa agenta - dla sieci wczytywanych
    public:
        Agent():x(0),y(0),r(1),state(0),pstate(0)
            {}
        void _setstate(double state);        //Bezposrednie nadawanie stanu - bez funkcji mapujacych
        void setpos(double x,double y,double r=-1);//Ustawianie pozycji i promienia. r=-1 --> pozostawia stary promien    
        void prepare_to_step();
        void add_to_delta(double input);
        void add_the_delta();
        void add_directly(double noise);
    friend class Swiat; //Musi miec bezposredni dostep do pol, zeby przypiac zrodla danych
    };

    class Connection
    {
		size_t start_node;   //Indeks wezla startowego
        size_t end_node;     //Indeks wezla koncowego
        double weight;       //Waga polaczenia
        double lastact;      //Ostatnio rejestrowana aktywnosc polaczenia
    public:
        Connection():start_node(-1),end_node(0),weight(0),lastact(0)
            {}
        void set(size_t s,size_t e,double w=0);   
        void clean_act(){lastact=0;}
	friend class Swiat; //Musi miec dostep do p�l, zeby przypiac zrodla danych
	};

private:
    wb_dynarray<Agent> agents;  //Lista agent�w
	wb_dynarray<Connection> connections; //Lista polaczen
    unsigned mode;//0 - bez polaczen , 1 - symetrycznie 2-niesymetryczne polaczenia
    unsigned dlugosc_logow; //Dlugosc buforow na statystyki symulacji
    unsigned reqested_number;//Zadana liczba agentow

    double m_init_st;   //Sredni stan poczatkowy
    double r_init_st;   //Odchylenie od sredniego stanu poczatkowego
    double m_of_noise;  //Sredni poziom szumu/sygnalu dodawanego do wezlow
	double r_of_noise;  //Odchylenie od sred. szumu/sygnalu dodawanego do wezlow
    double m_of_weight; //Srednia waga krawedzi
    double r_of_weight; //Odchylenie od sred. wagi krawedzi
    double asymmetry;   //Asymetria przekazywania komunikatow dodatnich wzgledem ujemnych
    
	struct_array_source<Agent,double>* pNodeX;      //Wspolrzedne wezlow w aranzacji
    struct_array_source<Agent,double>* pNodeY;      //------------//----------------
    struct_array_source<Agent,double>* pNodeR;      //Rozmiar wezla w aranzacji
    struct_array_source<Agent,double>* pNodeState;  //Aktywnosc wezlow
    struct_array_source<Agent,double>* pNodePreState;//Poprzednia aktywnosc wezlow
    struct_array_source<Agent,double>* pNodeDelta;  //Ostatni wplyw od innych

    struct_array_source<Connection,size_t>* pConnStart;//Indeksy pocz�tk�w linii laczacych wezly sieci
    struct_array_source<Connection,size_t>* pConnEnd;//Indeksy koncow linii laczacych wezly sieci
    struct_array_source<Connection,double>* pConnWeight;//Waga polaczenia
    struct_array_source<Connection,double>* pConnAcct;//Aktywnosc polaczenia
    
    generic_basic_statistics_source*	StateStat;
    fifo_source<double>*    MinStateLog;
    fifo_source<double>*    MeanStateLog;
    fifo_source<double>*    MaxStateLog;

    generic_basic_statistics_source*	AcctStat;
    fifo_source<double>*    MinAcctLog;
    fifo_source<double>*    MeanAcctLog;
    fifo_source<double>*    MaxAcctLog;

    //FUnkcje pomocnicze do modelu
    void _MakeCircle(double cx=0,double cy=0,double r=1,unsigned start=0,unsigned end=-1);  //Uklada agentow w kolko
    void _MakeStates(double mean,double max,unsigned start=0,unsigned end=-1); //Ustala agentom stany z rozkladu
    void _AddNoise(); //Dodaje "sobie" szum

    //Pomocnicze do statystyk itp
    void AllocSources(); //Tworzy zrodla danych
public:
    virtual 
        ~Swiat() {}
    
    Swiat(  unsigned  WhatMode,
        unsigned HowManyAgents,

        double im_init_st,  //Sredni stan poczatkowy
        double ir_init_st,   //Odchylenie od sredniego stanu poczatkowego
        double im_of_noise,  //Sredni poziom szumu/sygnalu dodawanego do wezlow
        double ir_of_noise,  //Odchylenie od sred. szumu/sygnalu dodawanego do wezlow
        double im_of_weight=0.5, //Srednia waga krawedzi
        double ir_of_weight=0.1, //Odchylenie od sred. wagi krawedzi
        double iasymmetry=1   //Asymetria przekazywania komunikatow dodatnich wzgledem ujemnych    
        ):
    dlugosc_logow(100000),
        mode(WhatMode),
        reqested_number(HowManyAgents),
        m_init_st(im_init_st),   //Sredni stan poczatkowy
        r_init_st(ir_init_st),   //Odchylenie od sredniego stanu poczatkowego
        m_of_noise(im_of_noise),  //Sredni poziom szumu/sygnalu dodawanego do wezlow
        r_of_noise(ir_of_noise),  //Odchylenie od sred. szumu/sygnalu dodawanego do wezlow
        m_of_weight(im_of_weight), //Srednia waga krawedzi  
        r_of_weight(ir_of_weight), //Odchylenie od sred. wagi krawedzi
        asymmetry(iasymmetry),      //Asymetria przekazywania komunikatow dodatnich wzgledem ujemnych
        world(LogName,50)   
    {
		pNodeX=NULL;pNodeY=NULL;pNodeR=NULL;pNodeState=NULL;pNodePreState=NULL;pNodeDelta=NULL;
        pConnStart=NULL;pConnEnd=NULL;pConnWeight=NULL;pConnAcct=NULL;
		StateStat=NULL;MinStateLog=NULL;MeanStateLog=NULL;MaxStateLog=NULL;
        AcctStat=NULL;MinAcctLog=NULL;MeanAcctLog=NULL;MaxAcctLog=NULL;
    }
    
    //Do ustawiania nowych parametrow modelu, przed reset?reinitialise
    void SetModelParameters(
        double im_init_st,  //Sredni stan poczatkowy
        double ir_init_st,   //Odchylenie od sredniego stanu poczatkowego
        double im_of_noise,  //Sredni poziom szumu/sygnalu dodawanego do wezlow
        double ir_of_noise,  //Odchylenie od sred. szumu/sygnalu dodawanego do wezlow
        double im_of_weight=0.5, //Srednia waga krawedzi
        double ir_of_weight=0.1, //Odchylenie od sred. wagi krawedzi
        double iasymmetry=1   //Asymetria przekazywania komunikatow dodatnich wzgledem ujemnych    
        )
    {
        m_init_st=im_init_st;   //Sredni stan poczatkowy
        r_init_st=ir_init_st;   //Odchylenie od sredniego stanu poczatkowego
        m_of_noise=im_of_noise;  //Sredni poziom szumu/sygnalu dodawanego do wezlow
        r_of_noise=ir_of_noise;  //Odchylenie od sred. szumu/sygnalu dodawanego do wezlow
        m_of_weight=im_of_weight; //Srednia waga krawedzi  
        r_of_weight=ir_of_weight; //Odchylenie od sred. wagi krawedzi
        asymmetry=iasymmetry;      //Asymetria przekazywania komunikatow dodatnich wzgledem ujemnych
    }
    
    
    //Trzeba wybrac ktoras z metod inicjalizacji sieci
    //int     /*def_*/mode=1; //0-bez polaczen, 1-symetryczne polaczenia 2-asymetryczne polaczenia
    void InitialiseNotConnected(unsigned HowManyAgents);
    void InitialiseFullyConnected(unsigned HowManyAgents);
    void InitialiseRandomConnected(unsigned HowManyAgents);
	void InitialiseFromWiesiekFile(const char* FileName);

    //Dodatkowe akcesory do zrodel statystycznych
    /*const*/ fifo_source<double>&    MeanStates() //Log srednich stanow. Bezs konst bo za duzo do przerabiania
        {return *MeanStateLog;};
    //... i inne kiedys
    //...
    
    //PROVIDED FOR COMPATYBILITY WITH BASE CLASS "world"
    // "user defined" actions for initialisation.
    virtual void initialize_layers();	
    //Wlasciwa implementacja kroku symulacji - do zaimplementowania
    virtual void simulate_one_step();	
	// user defined actions after read symulation state from file
    virtual void after_read_from_image();
	//Generuje podstawowe zrodla dla wbudowanego menagera danych
	virtual void make_basic_sources();
    //Wspolpraca z menagerem wyswietlania
	virtual void make_default_visualisation();//Tworzy domyslne "lufciki" i umieszcza w area_menager_base&  Menager
	//Aktualizacja zawartosci okna statusu po n krokach symulacji
    virtual void actualize_out_area(); 
};

inline void Swiat::Agent::prepare_to_step()
{
    this->pstate=this->state;
	this->delta=0;
}

inline void Swiat::Agent::add_to_delta(double input)
{
    delta+=input;
}

inline void Swiat::Agent::add_the_delta()
{   
    state=tanh(state+delta);//???
}

inline void Swiat::Agent::add_directly(double noise)
{
    state=tanh(state+noise);
}

void Swiat::simulate_one_step()
{
    for(unsigned i=0;i<agents.get_size();i++)
        agents[i].prepare_to_step();
    
	if(mode==1 || mode==2)//Fully connected or from file undirected
	{
        for(unsigned j=0;j<connections.get_size();j++)
        {
            double p1,p2,w=connections[j].weight;
            unsigned s=connections[j].start_node;
            unsigned e=connections[j].end_node;
            double St=agents[e].state;
            agents[s].add_to_delta(p1=(w*St*(St<0?asymmetry:1)));//Konflikt to wartosci ujemne - umownie oczywiscie
            St=agents[s].state;
            agents[e].add_to_delta(p2=(w*St*(St<0?asymmetry:1)));
			connections[j].lastact=wbrtm::max(p1,p2);  //Jaki� konflikt mimo �e jawinei nie ma using namespace wbrtm; TODO ?
        }
    }

    if(mode==3 || mode==4)//Random directed or from file directed
    {
        for(unsigned j=0;j<connections.get_size();j++)
        {
            double p,w=connections[j].weight;
            unsigned s=connections[j].start_node;
			unsigned e=connections[j].end_node;
            double St=agents[s].state;
            agents[e].add_to_delta(p=(w*St*(St<0?asymmetry:1)));//Konflikt to wartosci ujemne - umownie oczywiscie
            connections[j].lastact=p;
        }
    }
    
    if(mode>0)
    {
        for(unsigned i=0;i<agents.get_size();i++)   
            agents[i].add_the_delta();
    }
    
    _AddNoise();
}

void Swiat::_AddNoise()
{
    for(unsigned i=0;i<agents.get_size();i++)
    {
     double n=niby_gauss_max(m_of_noise,m_of_noise+r_of_noise);
     agents[i].add_directly(n);
    }
}

void Swiat::_MakeStates(double mean,double max,unsigned start,unsigned end/*=-1*/) //Ustala agentom stany z rozkladu
{
	if(end==unsigned(-1) )//SPECIALNE ZNACZENIE -1 "nie znam rozmiaru, we� i sprawd�"
		end=agents.get_size();
 
    for(unsigned i=start;i<end;i++)
    {
		agents[i]._setstate(0);//Bo moga byc stare wartosci
        agents[i].add_directly(niby_gauss_max(mean,mean+max));
    }
}

void Swiat::_MakeCircle(double cx,double cy,double r,unsigned start,unsigned end/*=-1*/)
{
	if(end==unsigned(-1)) //SPECIALNE ZNACZENIE -1 "nie znam rozmiaru, we� i sprawd�"
        end=agents.get_size();
    int ile=end-start;
    double krok=(2*M_PI)/ile;
    double ar=1;//domyslny promien agenta - trzeba setminmax() dla seri "R" !!!
    
    for(double angle=0;angle<(2*M_PI) && start<end;angle+=krok)
    {
        double x=cx+sin(angle)*r;
        double y=cy+cos(angle)*r;
        agents[start].setpos(x,y,ar);
        start++;
    }

    assert(start<=end);
}

void Swiat::InitialiseNotConnected(unsigned HowManyAgents)
{
    if(agents.get_size()!=HowManyAgents)//Ale jak nie rowne i reinicjalizacja to kicha!!!
        agents.alloc(HowManyAgents);
    //connections.alloc(1);
    //connections[0].set(0,0,0);//Dummy connection
    _MakeCircle();//Uklada wszystkich w domyslne kolo
    _MakeStates(m_init_st,r_init_st); //Ustala agentom stany z rozkladu
}

void Swiat::InitialiseFullyConnected(unsigned HowManyAgents)
{
    if(agents.get_size()!=HowManyAgents)//Ale jak nie rowne i reinicjalizacja to kicha!!!
        agents.alloc(HowManyAgents);    
    
    _MakeCircle();//Uklada wszystkich w domyslne kolo
    _MakeStates(m_init_st,r_init_st); //Ustala agentom stany z rozkladu

    unsigned how_many_con=((HowManyAgents*HowManyAgents)-HowManyAgents)/2; //( n^2-n )/ 2
    if(how_many_con!=connections.get_size())//Ale jak nie rowne i reinicjalizacja to kicha!!!
		connections.alloc(how_many_con);

	unsigned countcon=0; //zliczanie/przesuwanie
	for(unsigned i=1;i<HowManyAgents;i++)
    {
        for(unsigned j=0;j<i;j++)
        {
            double Weight=niby_gauss_max(m_of_weight,m_of_weight+r_of_weight);
            connections[countcon++].set(i,j,Weight);
        }
	}                                                  assert(countcon==how_many_con);
}

void Swiat::InitialiseRandomConnected(unsigned HowManyAgents)
{
    if(agents.get_size()!=HowManyAgents)//Ale jak nie rowne i reinicjalizacja to kicha!!!
        agents.alloc(HowManyAgents);    

    _MakeCircle();//Uklada wszystkich w domyslne kolo
    _MakeStates(m_init_st,r_init_st); //Ustala agentom stany z rozkladu
    
    unsigned how_many_con=(HowManyAgents*HowManyAgents)-HowManyAgents; //n^2-n
    if(how_many_con!=connections.get_size())//Ale jak nie rowne i reinicjalizacja to kicha!!!
            connections.alloc(how_many_con);

	unsigned countcon=0;//ZLICZANIE
	for(unsigned i=0;i<HowManyAgents;i++)
	{
        for(unsigned j=0;j<HowManyAgents;j++)
        if(j!=i)
        {
			double Weight=niby_gauss_max(m_of_weight,m_of_weight+r_of_weight);
            if(fabs(Weight)>0.000001)
                connections[countcon++].set(i,j,Weight);
        }
	}                                                  assert(countcon==how_many_con);
}


void Swiat::InitialiseFromWiesiekFile(const char* FileName)
{
	ifstream Input(FileName,ios::in | ios::nocreate);
	int HowManyAgents=INT_MAX;

	Input>>HowManyAgents;
														assert(HowManyAgents>=0);

	if(HowManyAgents>0 && HowManyAgents<1000) //DLACZEGO 1000 ??? TODO
		if(agents.get_size()==unsigned(HowManyAgents))//Rzut bo warning
        {//Juz bylo czytane
            _MakeStates(m_init_st,r_init_st); //Ustala agentom stany z rozkladu
			for(unsigned c=0;c<connections.get_size();c++)
				connections[c].clean_act();
            cerr<<"Network already read before. Only states were cleaned."<<endl;
        }
        else
		{
            agents.alloc(HowManyAgents); 
            Input.ignore(INT_MAX,'\n');
            for(int n=0;n<HowManyAgents;n++)
            {
                double X=-DBL_MAX;
                double Y=-DBL_MAX;
                double S=0;
                double R=1; //Mozna by uzalezni� od tekstu albo od warto�ci za tekstem (na razie ignorowanej)
                wb_pchar TmpStr;
                Input>>X>>Y>>S>>TmpStr;
                cerr<<X<<' '<<Y<<' '<<S<<' '<<TmpStr<<endl;
                Input.ignore(INT_MAX,'\n');
                agents[n].setpos(X,Y,R);
                agents[n]._setstate(S);//Bo moga byc stare wartosci
                if(X==-DBL_MAX || Y==-DBL_MAX || Input.fail())
                {
                    cerr<<"AT NODE #"<<n<<" AN ERROR OCCURED DURING DATA FILE READING"<<endl;
                    return;
                }

                if(1)//Tylko wtedy gdy stany nie s� dane w pliku. Na razie nie ma takiej mozliwosci
                _MakeStates(m_init_st,r_init_st); //Ustala agentom stany z rozkladu
            }

			Input >> ws;//old Input.eatwhite();

            unsigned countcon=0;
            connections.alloc(HowManyAgents*HowManyAgents);//Poczatkowo z zapasem
            while(!(Input.fail()) && !(Input.eof()))
            {
                int S=-INT_MAX;
                int E=-INT_MAX;
                double W=-DBL_MAX;        
				Input>>S>>E>>W;
                cerr<<S<<"-->"<<E<<' '<<W<<endl;
                Input.ignore(INT_MAX,'\n');
				Input>> ws ;//.eatwhite();//Jest nadzieja :) , �e bedzie EOF a nie \r na koncu czytania   TODO CHECK becouse changed
				connections[countcon++].set(S,E,W);
            }
            connections.trunc(countcon);
            if(Input.fail())
            {
                cerr<<"AT LINK #"<<countcon<<" AN ERROR OCCURED DURING DATA FILE READING"<<endl;
            }
        }
}

void Swiat::initialize_layers()
{  
   static first=1;//TYMCZASOWE WYLACZENIE NADMIARU WYDRUKOW!!!
   if(first)
		Log.GetStream()<<"CONFLICTS SIMULATION:";
   
   switch(mode)
   {
   case 0: this->InitialiseNotConnected(reqested_number); 
       Log.GetStream()<<"\nMode 0: Not connected"<<endl;
       cout<<"\nMode 0: Not connected"<<endl;
       break;
   case 1: this->InitialiseFullyConnected(reqested_number); 
       Log.GetStream()<<"\nMode 1: Undirected random weight connected"<<endl;
       cout<<"\nMode 1: Undirected random weight connected"<<endl;
       break;   
   case 2: this->InitialiseFromWiesiekFile(DefaultNetFileName);
       Log.GetStream()<<"\nMode 2: Undirected initialisation from file "<<DefaultNetFileName<<endl;
       cout<<"\nMode 2: Initialisation from file "<<DefaultNetFileName<<"(Undirected)"<<endl;
       //Symetrically, but from file
       break;
   case 4: this->InitialiseFromWiesiekFile(DefaultNetFileName);
       Log.GetStream()<<"\nMode 4: Directed initialisation from file "<<DefaultNetFileName<<endl;
       cout<<"\nMode 4: Initialisation from file "<<DefaultNetFileName<<"(Directed)"<<endl;
       //Assymetrically, but from file
       break; 
   default:
   case 3: this->InitialiseRandomConnected(reqested_number);
       Log.GetStream()<<"\nMode 3: Directed random weight connected"<<endl;
       cout<<"\nMode 3: Directed random weight connected"<<endl;
       break;   
   }

 
   	//...wydruk wartosci parametrow symulacji
   if(first)
	  Log.GetStream()
      <<"\nNumber of agents="<<Log.separator()<<agents.get_size()
      <<"\nNumber of connections="<<Log.separator()<<connections.get_size()
      <<"\n";
   first=0;//Koniec pierwszego wywolania //TYMCZASOWO!!! 
}

void Swiat::actualize_out_area()
{
    world::actualize_out_area();
    wb_pchar bufor(4000);
    bufor.prn("\nI=%g+-%g N=%g+-%g\n",m_init_st,r_init_st,m_of_noise,r_of_noise);
    OutArea->add_text( bufor.get_ptr_val() );
}


// user defined actions after read symulation state from file
void Swiat::after_read_from_image()
{
	//...???
}

void Swiat::AllocSources() //Tworzy zrodla danych
{
	pNodeX=new struct_array_source<Agent,double>(agents.get_size(),agents.get_ptr_val(),&Agent::x,"X" );      //Wspolrzedne wezlow w aranzacji
    pNodeY=new struct_array_source<Agent,double>(agents.get_size(),agents.get_ptr_val(),&Agent::y,"Y");      //------------//----------------
    pNodeR=new struct_array_source<Agent,double>(agents.get_size(),agents.get_ptr_val(),&Agent::r,"R");//Rozmiar wezla w aranzacji
    pNodeR->setminmax(0,1);//?DEBUG - promien jest staly na razie

	pNodeState=new struct_array_source<Agent,double>(agents.get_size(),agents.get_ptr_val(),&Agent::state,lang("Stan","State"));  //Aktywnosc wezlow
	pNodePreState=new struct_array_source<Agent,double>(agents.get_size(),agents.get_ptr_val(),&Agent::pstate,lang("Pop.Stan","PrevSt"));  //Dawna aktywnosc wezlow
	if(FixMinState<FixMaxState)
	{
		pNodeState->setminmax(FixMinState,FixMaxState);
		pNodePreState->setminmax(FixMinState,FixMaxState);
    }
    pNodeDelta=new struct_array_source<Agent,double>(agents.get_size(),agents.get_ptr_val(),&Agent::delta,"Delta");  //Dawna aktywnosc wezlow
    
    pConnStart=new struct_array_source<Connection,size_t>(connections.get_size(),connections.get_ptr_val(),&Connection::start_node,lang("Pocz�tki","Starts"));//Indeksy pocz�tk�w linii laczacych wezly sieci
    pConnEnd=new struct_array_source<Connection,size_t>(connections.get_size(),connections.get_ptr_val(),&Connection::end_node,lang("Ko�ce","Ends"));//Indeksy koncow linii laczacych wezly sieci
    pConnWeight=new struct_array_source<Connection,double>(connections.get_size(),connections.get_ptr_val(),&Connection::weight,lang("Wagi","Weights"));//Wagi polaczen
    pConnAcct=new struct_array_source<Connection,double>(connections.get_size(),connections.get_ptr_val(),&Connection::lastact,lang("Aktywno��","Acctivity"));//Aktywnosci polaczen

    StateStat=new generic_basic_statistics_source(pNodeState);  assert(StateStat!=NULL);//Statystyka stanow wezlow
    MeanStateLog=new fifo_source<double>(StateStat->Mean(),dlugosc_logow);assert(MeanStateLog!=NULL);//Fifo ze sredniej
    MinStateLog=new fifo_source<double>(StateStat->Min(),dlugosc_logow);assert(MinStateLog!=NULL);//Fifo z minimum
    MaxStateLog=new fifo_source<double>(StateStat->Max(),dlugosc_logow);assert(MaxStateLog!=NULL);//Fifo z maximum

    AcctStat=new generic_basic_statistics_source(pConnAcct);  assert(AcctStat!=NULL);//Statystyka stanow wezlow
    MeanAcctLog=new fifo_source<double>(AcctStat->Mean(),dlugosc_logow);assert(MeanAcctLog!=NULL);//Fifo ze sredniej
    MinAcctLog=new fifo_source<double>(AcctStat->Min(),dlugosc_logow);assert(MinAcctLog!=NULL);//Fifo z minimum
    MaxAcctLog=new fifo_source<double>(AcctStat->Max(),dlugosc_logow);assert(MaxAcctLog!=NULL);//Fifo z maximum
}

void Swiat::make_basic_sources()
{
	world::make_basic_sources();

	AllocSources();
	//Series ---> this->Sources
	Sources.insert(pNodeX); //1 oznacza, ze menager ma nie zwalniac
	Sources.insert(pNodeY);
	Sources.insert(pNodeR);
	Sources.insert(pNodeState);
	Sources.insert(pNodePreState);
	Sources.insert(pNodeDelta);

	Sources.insert(pConnStart);
	Sources.insert(pConnEnd);
	Sources.insert(pConnWeight);
	Sources.insert(pConnAcct);
    
	Sources.insert(StateStat);
	Sources.insert(MinStateLog);
	Sources.insert(MeanStateLog);
	Sources.insert(MaxStateLog);

	Sources.insert(AcctStat);
	Sources.insert(MinAcctLog);
	Sources.insert(MeanAcctLog);
    Sources.insert(MaxAcctLog);

    //I umieszczanie w logu tego co trzeba
    Log.insert(StateStat->Mean());
    Log.insert(StateStat->Min());
    Log.insert(StateStat->Max());
    Log.insert(StateStat->SD());

    Log.insert(AcctStat->Mean());
    Log.insert(AcctStat->Min());
    Log.insert(AcctStat->Max());
	Log.insert(AcctStat->SD());
}

void Swiat::make_default_visualisation() // area_menager_base& Lufciki     ?
{
											assert(this->HasAreaMenager());
	world::make_default_visualisation(); //Tworzy np. OutArea

	this->OutArea->set(0,0,250,250);

	graph* pom=NULL;
    
    float circle_max_size=(1.0f/agents.get_size())*0.5*M_PI;//*1.5*M_PI;// 2*Pi ??

	if(connections.get_size()>0)
    {
		net_graph* png;
		pom=png=new net_graph(250,0,500,250+3*char_height('X'),
			pNodeX,0,
            pNodeY,0,
            pConnStart,0,
            pConnEnd,0,
            
            pNodeState,0,//KOLORY - STAN
            pNodeR,0, //Rozmiary - promien
            NULL,0, //Arrows,1,
            pConnAcct,0,
            new circle_point(0.01f,circle_max_size),1
			);

        png->setbackground(default_dark_gray);
        //png->setdatacolors(256,511);
    }
    else
    {
        pom=new scatter_graph(250,0,500,250+3*char_height('X'),
			pNodeX,0,
            pNodeY,0,
            pNodeState,0,//KOLORY - STAN
            pNodeR,0, //Rozmiary - promien
            new circle_point(0.01f,circle_max_size),1);          
        pom->setbackground(default_dark_gray);
    }

    pom->setframe(128);
    pom->settitle(lang("MAPA SIECI","NETWORK MAPP"));
	this->MyAreaMenager().insert(pom);

    //Zaleznosc stanow nowych od poprzednich dla kazdego wez�a
	pom=new scatter_graph(this->MyAreaMenager().getwidth()-249,
						  this->MyAreaMenager().getheight()-250-3*char_height('X'),
						  this->MyAreaMenager().getwidth()-1,
						  this->MyAreaMenager().getheight()-1,
            pNodePreState,0,
            pNodeState,0,
            pNodeDelta,0
            );
    pom->setframe(64);
    pom->setbackground(default_light_gray);
    pom->settitle("Xn-i vs. Xn");
    //pom->series_info->setminmx();
	this->MyAreaMenager().insert(pom);

    //STATYSTYKA STAN�W 
    {
    data_source_base* data[4]={MinStateLog,MeanStateLog,MaxStateLog,NULL};
    pom=new sequence_graph(1,250,250,550,//domyslne wspolrzedne
							    3,data,
								//0/* Z reskalowaniem */);
							    1/*Wspolne minimum/maximum*/);
    assert(pom);
    pom->setframe(128);
    pom->settitle(lang("HISTORIA STAN�W",HISTofSTATES));
	this->MyAreaMenager().insert(pom);
    }

    //Statystyka aktywnosci
    {
    data_source_base* data[4]={MinAcctLog,MeanAcctLog,MaxAcctLog,NULL};
    pom=new sequence_graph(250,250+3*char_height('X')+1,500,550,
							    3,data,
								//0/* Z reskalowaniem */);
							    1/*Wspolne minimum/maximum*/);
	assert(pom);
    pom->setframe(128);
    pom->settitle(lang("HISTORIA AKTYWNO�CI",HISTofACCT));
	this->MyAreaMenager().insert(pom);
    }

    Sources.new_data_version(1,1);//Oznajmia seriom ze dane sie uaktualnily	(po inicjacji)
}

inline void Swiat::Agent::_setstate(double istate)     
//Bezposrednie nadawanie stanu - bez funkcji mapujacych
{
    state=istate;
}
inline void Swiat::Agent::setpos(double ix,double iy,double ir/*=-1*/)
{
    x=ix;y=iy;
    if(ir>=0) r=ir;
}
inline void Swiat::Connection::set(size_t s,size_t e,double w)
{
    start_node=s;end_node=e;weight=w;
}


//Klasa do przechowywania i wizualizowania danych spoza swiata swiata symulacji
//i/lub o poszczegolnych swiatach symulacji (dla roznych parametrow)
/////////////////////////////////////////////////////////////////////////////////
class MetaExperyment  
////////////////////
{
    class tangens//Klasa funkcyjna opakowujaca tangens
    {
    public:
        double operator () (double x) { return tan(x);}
    };
    
    class arcus_tangens//Klasa funkcyjna opakowujaca arcustangens
    {
    public:
        double operator () (double x){ return atan(x);}
    };
    
    class hiperbolic_tangens//Klasa funkcyjna opakowujaca tangens hiperboliczny
    {
    public:
        double operator () (double x){ return tanh(x);}
    };
    
    class sigmoida//Klasa funkcyjna opakowujaca funkcje sigmoidaln�
    {
    public:
        double operator () (double x) { return 1/(1+exp(-x));}
    };

    template<unsigned power>
    class pierwiastek_nieparzysty//Klasa funkcyjna opakowujaca nieparzyste pierwiastaki
    {
    public:
        double operator () (double x) 
        {
            if(x==0) return 0;
            else
            if(x>0)
                return pow(x,1.0/power);
            else
                return -pow(-x,1.0/power);

        }
    };

    class Point
    {
        double X,Y,Z;
    public:
        Point():X(-DBL_MAX),Y(-DBL_MAX),Z(-DBL_MAX)//Musza byc "missing"
        {}
        
        void Set(double ix,double iy,double iz)
        { X=ix;Y=iy;Z=iz;}
    
        friend class MetaExperyment;//Potrzebne do insertu do Sources
    };

    class Arrow
    {
        unsigned S,E;
        double W;
    public:
        Arrow():S(UINT_MAX),E(UINT_MAX),W(-DBL_MAX)//Musza byc "missing"
        {}

        void Set(unsigned is,unsigned ie,double iw)
        { S=is;E=ie;W=iw;} 

        friend class MetaExperyment;//Potrzebne do insertu do Sources
    };

    wb_dynarray<unsigned int> histogram_rnd;
    wb_dynarray<unsigned int> histogram_gau;   
    wb_dynarray<Arrow>  arrows;
    wb_dynarray<Point>  points;
    unsigned countexp;
    unsigned countpoints;
    //Zarzadzanie zrodlami danych 
    sources_menager			Sources;	//Zarzadca seri przekaznikowych
    //Obiekt rzadzacy zapisem historii
    logfile					Log;		//plik z zapisem histori symulacji
    
    void fill_rnds();

    public:
        MetaExperyment():Sources(20),Log(100,MetaExpFileName),
                         histogram_rnd(202),histogram_gau(202),
                         countexp(0),countpoints(0)
        {   fill_rnds();}
        
        void AllocForExperiments(unsigned HowManyExperiments)
        {
            points.alloc(3*HowManyExperiments);
            arrows.alloc(HowManyExperiments);            
        }

        void AddExperiment(double X,    //Tutaj bedzie to sredni szum
                           double Y0,double Y1,double Y2,//Poczatkowy sredni stan, srodkowy i koncowy
                           double Z0,double Z1,double Z2,//0,0,1 - strzalki maja puste konce, docelowy jest pelny
                           double W=1 //Waga strzalki dowolna
                           );      

       void tworz_lufciki(area_menager& Lufciki); //Generowanie lufcikow demostracyjnych meta-swiata        
       void save_for_spreadsheet(const char* filename);
};//end of META-SWIAT

void MetaExperyment::AddExperiment(double X,    //Tutaj bedzie to sredni szum
                                   double Y0,double Y1,double Y2,//Poczatkowy sredni stan, srodkowy i koncowy
                                   double Z0,double Z1,double Z2,//0,0,1 - strzalki maja puste konce, docelowy jest pelny
                                   double W //Waga strzalki dowolna
                                   )//
{
	assert(arrows.get_size()>countexp);//Tylko tu sie zgadza
    
    points[countpoints++].Set(X,Y0,Z0);//Punkt startowy
    
    double Y01=Y1;
	if(Y0>Y1)
        Y01=Y0-(ARROWS_DIV>0?(Y0-Y1)/ARROWS_DIV:0.05);
    else
        Y01=Y0+(ARROWS_DIV>0?(Y1-Y0)/ARROWS_DIV:0.05);
    //(Y1-Y0)/10;
    
    points[countpoints++].Set(X,Y01,Z1);//Punkt startowy
    //points[countpoints++].Set(X,Y1,Z1);//Punkt po kilku krokach
    
    arrows[countexp++].Set(countpoints-2,countpoints-1,W);
    
    points[countpoints++].Set(X,Y2,Z2);//Koncowy punkt
    Sources.new_data_version(1,1);//Oznajmia seriom ze dane sie uaktualnily	(po inicjacji)
}

void MetaExperyment::save_for_spreadsheet(const char* filename)
{
    ofstream Out(filename);
    if(!Out.is_open())
    {
        cerr<<"Can't open "<<filename<<" for writing."<<endl;
        perror("SYSTEM");
    }

    Out<<'X'<<'\t'<<"Attractor"<<'\t'<<"Field"<<'\t'<<"Heads"<<'\t'<<"Bases"<<endl;
    
    for(unsigned i=0;i<arrows.get_size();i++)
    {
        unsigned s=arrows[i].S;
        unsigned e=arrows[i].E;
        Out<<points[e+1].X<<'\t'<<points[e+1].Y<<'\t'<<""/*MISS*/ <<'\t'<<""/*MISS*/ <<'\t'<<""/*MISS*/<<endl;
        Out<<  points[s].X<<'\t'<<""/*MISS*/   <<'\t'<<points[s].Y<<'\t'<<""/*MISS*/ <<'\t'<<points[s].Y<<endl;
        Out<<  points[e].X<<'\t'<<""/*MISS*/   <<'\t'<<points[e].Y<<'\t'<<points[e].Y<<'\t'<<""/*MISS*/<<endl;        
    }
}

void MetaExperyment::tworz_lufciki(area_menager& Lufciki) //Generowanie lufcikow demostracyjnych meta-swiata
{
    int ret=0;    
    const BUTWIDTH=90;
    
    if(points.get_size()>0)
    {
        //Serie do meta-grafu!
        data_source_base* _X,*_Y,*_Z,*_S,*_E,*_Ayes,*_AC=NULL;

        Sources.insert(_X=new struct_array_source<Point,double>(points.get_size(),points.get_ptr_val(),&Point::X,"X"));
        assert(_X->get_missing()==-DBL_MAX);
        _X->setminmax(-1.2,1.2);
        Sources.insert(_Y=new struct_array_source<Point,double>(points.get_size(),points.get_ptr_val(),&Point::Y,"Y"));
        assert(_Y->get_missing()==-DBL_MAX);
        _Y->setminmax(-1.1,1.1);
        Sources.insert(_Z=new struct_array_source<Point,double>(points.get_size(),points.get_ptr_val(),&Point::Z,"Z"));
        assert(_Z->get_missing()==-DBL_MAX);
        
        Sources.insert(_S=new struct_array_source<Arrow,size_t>(arrows.get_size(),arrows.get_ptr_val(),&Arrow::S,lang("Pocz�tki","Starts")));//Indeksy pocz�tk�w linii laczacych wezly sieci
        assert(_S->get_missing()==UINT_MAX);
        Sources.insert(_E=new struct_array_source<Arrow,size_t>(arrows.get_size(),arrows.get_ptr_val(),&Arrow::E,lang("Ko�ce","Ends")));//Indeksy koncow linii laczacych wezly sieci
        assert(_E->get_missing()==UINT_MAX);
        Sources.insert(_Ayes=new function_source<constans<10> >(arrows.get_size(),0,1000000,"10",0,10));
        
        net_graph* png=new net_graph(501,0,Lufciki.getwidth()-BUTWIDTH-1,250+3*char_height('X'),
            _X,0,
            _Y,0,
            _S,0,
            _E,0,
            
            NULL,0,//KOLORY - STAN
            _Z,0, //Rozmiary - promien
            _Ayes,0, //Arrows,1,
            _AC,0,
            //new circle_point(0.00001f,0.01f),1
            new rhomb_point(0.00001f,DIAM_SIZE),1 
            );
        //png->setbackground(default_light_gray);
        png->settitle(META_GRAPH);
        Lufciki.insert(png);
    }
    
    //Wprowadzenie dodatkowych seri danych
    int _0=Sources.insert(	/*0*/new function_source<tangens>(225,-2*3.141595,2*3.141595,"tan(x)"));
    int _1=Sources.insert(	/*1*/new function_source<hiperbolic_tangens>(225,-2*3.141595,2*3.141595,"tanh(x)"));
    int _2=Sources.insert(	/*2*/new function_source<arcus_tangens>(225,-2*3.141595,2*3.141595,"atan(x)"));
    int _3=Sources.insert(	/*3*/new function_source<sinus>(225,-2*3.141595,2*3.141595,"Sin(x)"));
    int _4=Sources.insert(	/*4*/new function_source<cosinus>(225,-2*3.141595,2*3.141595,"Cos(x) ",-1,1));//Podane dokladne min i max
    int _5=Sources.insert(	/*5*/new array_source<unsigned int>(202,histogram_rnd.get_ptr_val(),"rnd(-100,100)"));
    int _6=Sources.insert(	/*6*/new array_source<unsigned int>(202,histogram_gau.get_ptr_val(),"gaus()"));
    int _7=Sources.insert(	/*7*/new function_source<sigmoida>(225,-2*3.141595,2*3.141595,"1/(1+exp(-x))"));
    int _8=Sources.insert(	/*8*/new function_source<pierwiastek_nieparzysty<3> >(225,-2*3.141595,2*3.141595,"pow(x,1/3)"));
    int _9=Sources.insert(	/*9*/new function_source<pierwiastek_nieparzysty<9> >(225,-2*3.141595,2*3.141595,"pow(x,1/9)"));
    graph* pom;
    
    unsigned BUTHEIGHT=unsigned(char_height('X')*1.2);
    pom=new sequence_graph(SWIDTH-BUTWIDTH,0,SWIDTH,BUTHEIGHT,6,
        Sources.make_series_info(_0,_1,_2,_7,_8,_9,-1).get_ptr_val(),
        2,//MODE 2 - MIN & MAX PROVIDED
        -3,3
        );    
    
	assert(pom!=NULL);
	pom->settitle(lang("FUNKCJE","FUNCTIONS"));
    pom->setframe(20);
    Lufciki.insert(pom);
    
    pom=new bars_graph(SWIDTH-BUTWIDTH,BUTHEIGHT,SWIDTH,2*BUTHEIGHT,Sources.get(_5));            assert(pom!=NULL);
    pom->settitle("TEST RND");
    pom->setframe(20);
    Lufciki.insert(pom);
    
    pom=new bars_graph(SWIDTH-BUTWIDTH,2*BUTHEIGHT,SWIDTH,3*BUTHEIGHT,Sources.get(_6));     assert(pom!=NULL);
    pom->settitle("TEST PSEUDO-GAUSS");
    pom->setframe(20);
    Lufciki.insert(pom);
    
    Sources.new_data_version(1,1);//Oznajmia seriom ze dane sie uaktualnily	(po inicjacji)
}


void MetaExperyment::fill_rnds()
{
    histogram_rnd.fill(0);    histogram_gau.fill(0);
    
    for(int i=0;i<100000;i++)
    {
        int x=TheRandG.Random(201);
        histogram_rnd[x]++;
        double y=niby_gauss_max(100,200);        
        histogram_gau[unsigned(y)]++;
    }
}

/*  G��WNE OBIEKTY I OGOLNA FUNKCJA MAIN  */
/******************************************/


int main(int argc,const char* argv[])
{
//int i=0;
//int cont=1;//flaga kontynuacji
//int std=0;
cout<<SYMULATION_NAME<<endl;
cout<<"Use -help for graphics setup information,\nor HELP for information about available parameters."<<endl;
if(OptionalParameterBase::parse_options(argc,argv,Parameters,sizeof(Parameters)/sizeof(Parameters[0])))
    return 1;
Swiat MojSwiat( //Model symulacyjny z roznymi parametrami
               def_mode,//=2; //0-bez polaczen, 1-symetryczne polaczenia 2-asymetryczne polaczenia
               def_num_of_nodes,//=50; //Ile jest wezlow 

               def_m_init_st,//=0,  //Sredni stan poczatkowy
               def_r_init_st,//=0.5,   //Odchylenie od sredniego stanu poczatkowego
               def_m_of_noise,//=0,  //Sredni poziom szumu/sygnalu dodawanego do wezlow
               def_r_of_noise,//=0.5,  //Odchylenie od sred. szumu/sygnalu dodawanego do wezlow
               def_m_of_weight,//=0.5, //Srednia waga krawedzi
               def_r_of_weight,//=0.1 //Odchylenie od sred. wagi krawedzi
               def_asymmetry
               );
MetaExperyment MojExperyment; //Obsluga meta-eksperymentu dla wielu ustawien symulacji
main_area_menager Lufciki(100,SWIDTH,SHEIGHT); //Zarzadzanie ekranem
if(!Lufciki.start(SYMULATION_NAME,argc,argv,1))
{
    printf("%s\n","Can't initialize graphics");
	exit(1);
}
else
//Utworzenie sensownej nazwy pliku(-�w) do zrzutow ekranu
{
	wb_pchar buf(strlen(SCREENDUMPNAME)+20);
    buf.prn("%s_%ld",SCREENDUMPNAME,time(NULL));
	Lufciki.set_dump_name(buf.get());
}

//Przygotowanie danych i Swiata symulacji
///////////////////////////////////////////
//inicjalizacja globalnego randomizer�w 
TheRandG.Reset();
TheRandSTDC.Reset();


MojSwiat.set_max_iteration(iMaxIterations);//Ile najwiecej krokow
MojSwiat.set_input_ratio(iViewRatio);   //Jak czesto wizualizowac
MojSwiat.set_log_ratio(iLogRatio);      //Jak czesto zapisywac do logu
//MojSwiat.set_history_stream(HistName);  //Nazwa pliku z pelna historia symulacji
MojSwiat.set_history_stream(NULL);      //Nie bedzie histori symulacji

cout<<SYMULATION_NAME<<": INITIALISED."<<endl;
cout<<"Ctrl-I for HELP, Ctrl-Q to quit, Ctrl-A to start symulation."<<endl;

if(MultiExp<2)
{
	MojSwiat.initialize(&Lufciki);
    MojExperyment.tworz_lufciki(Lufciki);       //Serie i Lufciki testowe spoza wlasciwej symulacji
    Lufciki.replot();
    Lufciki.process_input();//Pierwsze zdazenia. Koncza sie po ctrl-B	
    //GLOWNA PETLA SYMULACJI
    cout<<SYMULATION_NAME<<": STARTED."<<endl;
    MojSwiat.simulation_loop(0/*wylaczone automatyczne wychodzenie po zakonczeniu symulacji*/);
    cout<<SYMULATION_NAME<<": CLOSING."<<endl;
}
else
{     
    wb_pchar dump_name(2048);
    dump_name.prn("con1-T%d-ri_%g-rn_%g-w_%g(%g)-Ax%g-%dnod#",
                         def_mode,
                               def_r_init_st,
                                     def_r_of_noise,
                                           def_m_of_weight,
                                              def_r_of_weight,
                                                    double(def_asymmetry),
                                                         def_num_of_nodes);  
    
	int symulacja=0;  //A� tyle ich nie b�dzie, �eby potrzebne by�o unsigned

    for(double StartMeanState=defm_start_mean_state;StartMeanState<=defm_end_mean_state;StartMeanState+=(defm_end_mean_state-defm_start_mean_state)/MultiExp)//Podwojna petla
        for(double NoiseMean=defm_start_noise_mean;NoiseMean<=defm_end_noise_mean;NoiseMean+=(defm_end_noise_mean-defm_start_noise_mean)/MultiExp)//Po stanie poczatkowym i szumie
        {  
            //Niestety reinicjalizacja bo domyslne parametry to nie to!
            MojSwiat.SetModelParameters(           
                StartMeanState,//Sredni stan poczatkowy
                def_r_init_st,//Odchylenie od sredniego stanu poczatkowego
                NoiseMean,//Sredni poziom szumu/sygnalu dodawanego do wezlow
                def_r_of_noise,//Odchylenie od sred. szumu/sygnalu dodawanego do wezlow
                def_m_of_weight,//=0.5, //Srednia waga krawedzi
                def_r_of_weight,//=0.1 //Odchylenie od sred. wagi krawedzi
                def_asymmetry
                );   
            
            if(symulacja==0) //Jak piersza z symulacji to musimy poustawiac wszystkie "pomoce naukowe"
            {
                MojExperyment.AllocForExperiments((MultiExp+1)*(MultiExp+1));//Z lekkim zapasem 
                MojSwiat.initialize(&Lufciki);                
                MojExperyment.tworz_lufciki(Lufciki);           //Serie i Lufciki testowe spoza wlasciwej symulacji  
                
                int statusWin=Lufciki.search("STATUS");
                int histWin=Lufciki.search(HISTofSTATES);//HISTofSTATES="HISTORY OF THE STATE" HISTofACCT="HISTORY OF THE ACTIVITY"
                int histAcc=Lufciki.search(HISTofACCT);
                int metaWin=Lufciki.search(META_GRAPH);
                wb_dynarray<int> co(size_t(4),statusWin,histWin,histAcc,metaWin);
                Lufciki.tile(co);
                MojSwiat.set_max_iteration(iMaxIterRep);
                MojSwiat.set_log_ratio(10);
                Lufciki.enable_background();
                Lufciki.maximize(Lufciki.search("STATUS"));
            }
            else
            {
                MojSwiat.restart();//A jak nastepne to juz wystarczy tylko zrestartowac i gotowe
            }
            
            //GLOWNA PETLA SYMULACJI
            symulacja++;
            Lufciki.set_dump_name(dump_name.get(),symulacja*100);
            MojSwiat.MyLogStream()<<"BEGIN OF REP #"<<symulacja
                <<" M.of.INITIAL=\t"<<StartMeanState<<"\tM.of.NOISE=\t"<<NoiseMean<<endl;
            cout<<SYMULATION_NAME<<": SYMULATION "<<symulacja<<" STARTED ."<<endl;
            
            MojSwiat.simulation_loop(1);
            if(!Lufciki.should_continue()) goto USER_ABORT;

            cout<<SYMULATION_NAME<<": SYMULATION "<<symulacja<<" DONE ."<<endl;
            
            //Dodanie wynikow do listy eksperymentow
            double Y0=MojSwiat.MeanStates().get(unsigned(0));
            double YT=MojSwiat.MeanStates().get(unsigned(1));
            double YF=MojSwiat.MeanStates().get(unsigned(iMaxIterRep-1));
            MojExperyment.AddExperiment(NoiseMean,Y0,YT,YF,0,0.1,iMaxIterRep);

            if(symulacja<MultiExp-1)
            {
                MojSwiat.MyLogStream()<<"END OF REP #"<<symulacja<<endl;
            }
        }
        
        cout<<SYMULATION_NAME<<": MULTI-SYMULATION FINISHED ."<<endl;
        //Lufciki.need_confirmation_before_clean();
        dump_name.add(".%s","out");
        MojExperyment.save_for_spreadsheet(dump_name.get());
USER_ABORT:
        Lufciki.maximize(Lufciki.search(META_GRAPH));
        Lufciki.disable_background();
        Lufciki.run_input_loop();           
}

//I po zabawie
printf("Do widzenia!!!\n");
return 0;
//Gdzies tu,albo troche dalej destruktory...
}

