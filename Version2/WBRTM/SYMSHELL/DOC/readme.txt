SYMSHELL 0.91
(Zaprojektowa� i zaimplementowa� Wojciech Borkowski)
�
PRZENO�NY FUNKCJE GRAFICZNE DLA PROSTYCH APLIKACJI C/C++
	Problem przeno�no�ci aplikacji pomi�dzy r�nymi platformami sprz�towymi i systemami operacyjnymi jest zmor� m�cz�c� programist�w od zarania dziej�w informatyki. J�zyki C i C++ nie s� pod tym wzgl�dem najgorsze bowiem istniej� ich og�lnie przyj�te standardy pozwalaj�ce uzyska� przeno�no�� na poziomie kodu �r�d�owego o ile tylko istnieje kompilator C/C++ na dan� platform�. K�opot z tym ze standardy te nie obejmuj� bardziej zaawansowanych operacji wej�cia/wyj�cia nawet dotycz�cych terminala/ekranu tekstowego, nie m�wi�c ju� o operacjach graficznych. Tzw. standardowe wej�cie/wyj�cie pozostaje ci�gle na poziomie "teletype" jako najmniejszego wsp�lnego mianownika. Je�li potrzebne jest co� wi�cej - np. proste wy�wietlenie paru punkt�w czy kresek na ekranie czy wy�wietlenie ramki z tekstem na �rodku(!) ekranu to de facto nawet uzyskanie aplikacji przeno�nej pomi�dzy r�nymi systemami "unix'owymi" nie jest zadaniem ca�kiem trywialnym. Co dopiero gdy program musi mie� wersje pracuj�ce pod DOS'em, Windows , X-windows czy unix'owym terminalu tekstowym!
Dla du�ych (i bogatych) firm softwarowych czy innych zespo��w programist�w istniej� rozwi�zania w postaci gotowych bibliotek GUI o zapewnionej przeno�no�ci. Nawet MFC Microsoftu posiada (podobno) wersje na platform� unix/Motif. S� to jednak rozwi�zania kosztowne, zazwyczaj niedost�pne w postaci kodu �r�d�owego co automatycznie ogranicza ilo�� potencjalnych platform. Przede wszystkim s� to jednak rozwi�zania bardzo kosztowne i zwykle zbyt du�ego kalibru by obci��a� nimi proste aplikacje. 
	Poniewa� odk�d zacz��em programowa� w C/C++ , robi� to na co najmniej dw�ch platformach, a ostatnio na 5(!), postanowi�em rozwi�za� ten problem na swoje potrzeby. Dopracowa�em si� zestawu modu��w prostego interface'u graficznego razem obs�uguj�cych, i DOS, i MS Windows, i unix/X-windows. Mo�liwe jest tez stosunkowo �atwe dopisanie kolejnych.
�
SK�AD PAKIETU
Maksymalna zawarto�� dystrybucji pakietu obejmuje w obecnej wersji 0.9 nast�puj�ce pliki:
KATALOG ROOT:
*platform.h - plik nag��wkowy s�u��cy znormalizowanej detekcji u�ytego kompilatora i platformy.
*symshell.h - plik nag��wkowy interface'u . Wszystkie modu�y implementacyjne musz� by� z nim zgodne.
*errorent.c - standardowa definicja zmiennej WB_error_enter_before_clean.
*inne pochodzace z powi�zanych dystrybucji np graphs.
KATALOG X11:
*symshx11.c - plik implementuj�cy funkcje SYMSHELL'a na platformie X-windows
*icon.h - plik nag��wkowy dla ikony aplikacji w wersji X-windows
*wbx11ico.c - plik ze standardow� definicj� ikony dla aplikacji w wersji X-windows
KATALOG WIN:
*symshmsc.rc - wzorcowy plik zasob�w dla aplikacji w wersji Win32 kompatybilny MS Visual C++ 4.0
*symshwin.c - plik implementuj�cy funkcje SYMSHELL'a na platformie Win32 (testowany na Windows NT i Windows 95)
*symshwin.h ,win16ext.h, resources.h - lokalne definicje niezb�dne do kompilacji modu�u symshwin.c i jego zasob�w.
*symshwin.ico - standardowa ikona aplikacji dla wersji Win32
*symshwin.rc - wzorcowy plik zasob�w dla aplikacji w wersji Win32 kompatybilny z Borland 5.0
*winmain.c - plik przekszta�caj�cy wywo�anie funkcji WinMain() aplikacji GUI Win32 na wywo�anie standardowej funkcji main(). Niekonieczny dla aplikacji CONSOLE Win32.
�
KATALOG GRX:
*symshgrx.c - plik implementuj�cy funkcje SYMSHELL'a na platformy TURBOC i DJGPP z u�yciem biblioteki graficznej GRX lub GRX2.0
KATALOG TXT:
*symshcur.c - plik implementuj�cy funkcje SYMSHELL'a w pe�noekranowym trybie terminala tekstowego. Oczywi�cie rozdzielczo�ci s� rz�du 80x25 lub 132x40. W obecnej wersji nie zawiera juz w pelni funkcjonalnej implementacji.
*symshtxt.c - atrapa implementacji pos�uguj�ca si� (prawie) standardowym wej�ciem/wyj�ciem i wyprowadzaj�ca na ekran terminala jedynie informacje tekstow�. Mo�e te� by� zupe�nie "g�ucha" co umo�liwia �atwe przerobienie (przez relinkowanie) aplikacji graficznej na zadanie obliczeniowe pracuj�ce w tle.
KATALOG BIN:
*BC5.0/symshwin.obj,winmain.obj - skompilowany modu� w wersji dla Win32 kompilatora Borland 5.0
*DJGPP/symshx11.o - skompilowany modu� w wersji dla DJGPP (32 bit DOS)
*GRX/symshgrx.o - skompilowany modu� w wersji dla DJGPP 2.0 . Biblioteka GRX 2.0 
*IRIX/symshx11.o - skompilowany modu� w wersji dla systemu SG IRIX 6.x
*LINUX/symshx11.o - skompilowany modu� dla systemu LINUX (i386)
*MSC4.0/symshwin.obj,winmain.obj - skompilowane modu�y w wersji dla Win32 kompilatora MS Visual C++ 4.0.
*MSVC4.0/symshmsc.res - skompilowany wzorcowy plik zasobow.
*TC/symshgx1.obj, TC/symshgx1.obj - skompilowany modu� w wersji dla DOS BC/TC Bibliotek� GRX dla DOS trzeba mie� z innego �r�d�a.
zawarta jest w pe�nej dystrybucji DJGPP.
KATALOG TESTS:
*ro�norodne programy testowe i demonstracyjne.
KATALOG DOCS:
*pliki dokumentacji w ro�nych formatach.
KATALOG GRAPHS:
*Pliki obiektowej biblioteki na��dzi programowania symulacji. Opisane w plikach
dokumentacyjnych GRAPHS.RTF lub analogicznych.
�
�
Jak dotychczas nie zaimplementowa�em SYMSHELL'a w grafice BGI Borlanda i sam ju� tego nie zrobi�, ale je�li kto� b�dzie mia� ochot� to ch�tnie do��cz� do dystrybucji, jak r�wnie� inne, o kt�rych dotychczas nie pomy�la�em lub nie mia�em mo�liwo�ci np. ma MacOS. Chwilowo nie mam dost�pu do Linux'a wi�c nie proponuje modu��w skompilowanych dla tego systemu jednak wersje CURSES i X11 powinny da� si� tam skompilowa� bez problem�w.
UWAGA: 
Nie ka�da wersja dystrybucyjna pakietu zawiera wszystkie pliki. W szczeg�lno�ci wersje shareware nie zawieraj� kod�w �r�d�owych dla modu��w Win32 i X11, a wersje demo nie zawieraj� nie wygasaj�cych (bezterminowych) modu��w skompilowanych dla tych platform.
�
OPIS INTERFACE'U SYMSHELL
	SYMSHELL jest zestawem funkcji implementowanych w j�zyku C, ale oczywi�cie mo�liwych do u�ycia r�wnie� w C++ . Jego og�lna filozofia wywodzi si� z prostych program�w symulacyjnych pisanych pod DOS'em, st�d nazwa, ale nadaje si� te� do pisania prostych aplikacji przetwarzaj�cych dane, wizualizuj�cych je, a tak�e innych, je�li autorowi zale�y na zachowaniu przeno�no�ci. Jedynym warunkiem jest zachowanie og�lnej struktury aplikacji wg. nast�puj�cego schematu:
	1)ustalenie parametr�w pracy SYMSHELL'a
	2)otwarcie trybu/okna graficznego funkcj�
		init_plot(...)
	3)g��wna p�tla aplikacji u�ywaj�ca funkcji SYMSHELL'a
	 Dla poprawnej pracy. szczeg�lnie na platformach okienkowych aplikacja MUSI 	
	 u�ywa� funkcji input_ready() , get_char() i flush_plot()
	 oraz reagowa� na znaki specjalne:
		\b - zdarzenie myszy 
		\r - odrysowanie ekranu
		EOF - koniec wej�cia SYMSHELL'a 
	4)Zamkni�cie grafiki funkcja close_plot()
Funkcje s� opisane w kolejno�ci w jak w pliku nag��wkowym "symshell.h" :
Funkcje konfiguruj�ce i zarz�dzaj�ce:
int mouse_activity(int yes);
Ustala czy mysz ma by� obs�ugiwana przez aplikacje. Zwraca poprzedni stan flagi. Je�li yes=0 i je�li aplikacja jest jedyn� "w�a�cicielk�" ekranu to kursor nie pojawi si� lub zostanie wygaszony. W przeciwnym wypadku aplikacja jedynie przestaje odbiera� zdarzenia myszy. Je�li aplikacja przed otwarciem okna nie zadeklaruje czy chce u�ywa� myszy to mo�e si� ona nie da� zaaktywowa�. Rozs�dne jest wy��czenie aktywno�ci myszy na czas odbudowywania ekranu.
void set_background(unsigned char color);
Ustala indeks koloru do czyszczenia ekranu. Domy�lnym kolorem jest kolor 0 - czarny. Je�li aplikacja nie zadeklaruje koloru t�a przed otwarciem okna lub zmienia go po otwarciu to efekty mog� by� po�owiczne.
void buffering_setup(int yes);
Prze��czanie buforowania okna w pami�ci. Je�li yes=0 to ka�da operacja rysowania odbywa si� bezpo�rednio na ekranie (chyba �e to niemo�liwe). Je�li buforowanie jest w��czone to od�wie�enie zawarto�ci ekranu odbywa si� dopiero w momencie wywo�ania funkcji flush_plot(). W obecnej implementacji buforowanie mo�na w��czy� jedynie przed otwarciem okna graficznego. W �rodowiskach wielookienkowych zmniejsza to ilo�� przypadk�w kiedy konieczne jest odrysowanie ekranu przez aplikacje. W �rodowisku jednoekranowym tryb z buforowaniem mo�e nie wsp�dzia�a� w�a�ciwie z obs�ug� myszy - szczeg�lnie przy cz�stym od�wie�aniu ekranu. Na szcz�cie rzadko istnieje konieczno�� pisania takich aplikacji.
void shell_setup(char* Title, int Iargc, char* Iargv[]);
Przekazanie parametr�w wywo�ania programu (funkcji main) do SYMSHELL'a . Pe�n� list� parametr�w rozpoznawan� przez dan� implementacje mo�na uzyska� podaj�c parametr -h lub -help. Parametr Title s�u�y do ustalenia tytu�u okna je�li dana platforma dopuszcza tak� mo�liwo��. Funkcja ta powinna by� wywo�ywana przed funkcj� init_plot(). Je�li nie zostanie wywo�ana to przyjmowane s� warto�ci domy�lne. 
Akceptowane przez SYMSHELL parametry wywo�ania maj� znormalizowan� posta� :
	-nazwa+ lub -nazwa- .Pe�na lista zawiera:
	-input[+-] ->wy��czanie odbioru wej�cia. Tylko wersja tekstowa.
	-output[+-] ->wy��czanie standardowego wyj�cia. Tylko wersja tekstowa
	-mouse[+-] ->prze��czenie odbioru myszy.
	-async[+-] ->prze��czenie obs�ugi myszy w tryb asynchroniczny. Tylko GRX.
	-buffered[+-] ->prze��czanie buforowania wyj�cia graficznego.
	-traceenv[+-] ->w��czenie tekstowego �ledzenia zdarze� (events). Tylko w X11.
	-mapped[+-] ->w��czenie trybu podw�jnego zapisu w �rodowiskach okienkowych. Z punktu widzenia aplikacji jest on niezauwa�alny, usprawnia jednak od�wie�anie ekranu zwalniaj�c w wi�kszo�ci przypadk�w aplikacj� z tego obowi�zku. Jednak ka�da operacja ekranowa jest oko�o dwa razy wolniejsza bowiem odbywa si� jednocze�nie na ekranie i na buforze w pami�ci.
void init_plot(int a, int b, int ca, int cb);
Uruchomienie trybu graficznego lub semigraficznego lub otwarcie graficznego okna aplikacji.
a - wymagana szeroko�� ekranu w pikselach, b - wymagana wysoko�� ekranu w pikselach, 
ca - ilo�� dodatkowych kolumn tekstu (w znakach) po prawej stronie okna,
cb - ilo�� dodatkowych wierszy tekstu (w znakach) u do�u okna.
Pe�ny wymagany rozmiar ekranu powstaje ze zsumowania tych warto�ci i jest zale�ny od rozmiaru fontu stosowanego przez dan� implementacj�.
Faktyczny rozmiar okna mo�e by� wi�kszy od wymaganego, szczeg�lnie w implementacjach pe�noekranowych.
Je�li platforma dopuszcza zmian� rozmiaru okna przez u�ytkownika aplikacji to fakt tej zmiany jest w miar� mo�liwo�ci maskowany przed aplikacj� przez zwielokrotnianie rozmiaru punktu, ale tylko do warto�ci ca�kowitych.
	Trzeba mie� na uwadze �e czasem okno graficzne nakrywa si� z oknem tekstowym odbieraj�cym standardowe wyj�cie, czasem okno graficzne uniemo�liwia prac� standardowego wyj�cia, czasem (Windows) mo�liwe jest takie zbudowanie aplikacji ze jest ona w og�le pozbawiona funkcjonalnego standardowego wej�cia/wyj�cia.
void flush_plot();
Uzgodnienie zawarto�ci ekranu z operacjami graficznymi SYMSHELL'a . Niekt�re �rodowiska np X11 czy CURSES wymagaj� stosowania tej funkcji do prawid�owej pracy, w innych jest ona niezb�dna w trybie "buffered". W celu zachowania przeno�no�ci nie mo�na zaniedbywa� wywo�ywania tej funkcji, najlepiej pod koniec logicznego bloku wywo�a� graficznych. Zbyt cz�ste wywo�ania mog� jednak powodowa� znaczny spadek wydajno�ci aplikacji poniewa� mog� by� kosztowne.
void close_plot(); 
Zamkni�cie grafiki lub semigrafiki lub okna graficznego. Funkcj� t� nale�y wywo�ywa� dla przyzwoito�ci na ko�cu programu, jednak prawid�owa implementacja SYMSHELL'a umieszcza j� w kolejce funkcji atexit w trakcie wywo�ania open_plot() co zazwyczaj jest wystarczaj�ce. Wewn�trz funkcji close_plot sprawdzana jest zmienna zadeklarowana jako:
	extern int WB_error_enter_before_clean/* =0 */;
Je�li ma ona warto�� r�n� od 0 to funkcja interaktywnie prosi u�ytkownika o potwierdzenie. Zmienna ta musi by� gdzie� zdefiniowana w kodzie aplikacji lub nale�y dolinkowa� modu� errorent.c. Funkcj� t� mo�na bezpiecznie wywo�ywa� wielokrotnie, a nawet wywo�ywa� j� mimo niepowodzenia funkcji open_plot(). Tylko pierwsze wywo�anie nast�puj�ce po zako�czonym sukcesem otwarciu trybu graficznego wykonuje jakiekolwiek czynno�ci.
Funkcje odczytuj�ce parametry wy�wietlania:
int screen_height(); 
int screen_width(); 
Ostateczne rozmiary okna po przeliczeniach parametr�w funkcji init_plot() zawieraj�ce tak�e marginesy tekstowe. Pozwalaj� na pozycjonowanie w okolicach prawego dolnego rogu ekranu.
int char_height(char znak);
int char_width(char znak);
Aktualne rozmiary znaku w pikselach SYMSHELL'a, potrzebne do pozycjonowania tekstu. Rozmiar czcionki mo�e pozornie male� z punktu widzenia aplikacji, je�li u�ytkownik powi�kszy okno graficzne. Nie nale�y ich wi�c zapami�tywa� pomi�dzy wywo�aniami funkcji input_ready(), get_char(), flush_plot() .
Funkcje do czytania wej�cia:
int input_ready(); 
Nieblokuj�ca funkcja sprawdzaj�ca czy jest dost�pne wej�cie. Je�li funkcja zwraca 1 to jest pewno�� �e nast�puj�ce potem wywo�anie funkcji get_char() nie mo�e zastopowa� aplikacji.
int get_char(); 
Funkcja odczytywania znak�w sterowania. Znaki '\r' (carret return) , '\b' (bell) i EOF maj� specjalne znaczenie. Funkcja mo�e zwraca� warto�ci wi�ksze od 255 jako kody znak�w niestandardowych klawiatury (na razie niezdefiniowane) oraz informacje o wybraniu przez u�ytkownika pozycji z menu (na razie tylko Windows).
Reakcj� aplikacji na znak EOF powinno by� zako�czenie trybu graficznego, reakcj� na znak '\r' powinna by� odbudowa (odrysowanie) ekranu, a reakcj� na znak '\b' powinno by� wywo�anie funkcji get_mouse_event(...). Je�li aplikacja nie czyta odpowiednio cz�sto wej�cia lub nie zareaguje prawid�owo na '\r' to efektu wizualne mog� by� dziwne. Brak wywo�ania funkcji get_mouse_event(...) mo�e powodowa� utrat� zdarzenia myszy lub powt�rne zwr�cenie przez funkcj� get_char() znaku '\b'.
int get_mouse_event(int* xpos, int* ypos, int* click);
Przepisuje warto�ci ostatniego zdarzenie myszy na zmienne kt�rych adresy pobiera z parametr�w wywo�ania. 
	xpos - adres zmiennej na kt�r� przepisana zostanie wsp�rz�dna pozioma kursora.
	ypos - adres zmiennej na kt�r� przepisana zostanie wsp�rz�dna pionowa kursora.
	click -	adres zmiennej na kt�r� przepisana zostanie warto�� stanu przycisk�w myszy;
bit o wadze 1 oznacza przycisk lewy, bit o wadze 2 przycisk prawy, a bit o wadze 4 �rodkowy.
Funkcje wyprowadzaj�ce tekst na ekran graficzny:
void print(int x, int y, char* format,...); 
void printc(int x, int y,
	unsigned char foreground, unsigned char background,
	char* format,...);
Funkcje wyprowadzaj�ce tekst dzia�aj�ce analogicznie do standardowej funkcji printf.
Parametry x i y oznaczaj� po�o�enie lewego g�rnego rogu pierwszego znaku; "print" wyprowadza tekst w kolorach domy�lnych - zazwyczaj czarno na bia�ym, "printc" w kolorach o indeksach podanych parametrami "foreground" i "background". Aplikacja nie ma wp�ywu na rozmiar czcionki, mo�e jednak policzy� zawczasu rozmiar tekstu. (W Windows wyniki takich oblicze� s� przybli�one).
Funkcje rysuj�ce:
Zestaw tzw prymityw�w graficznych. Stosunkowo �atwe by�oby jego rozbudowywanie. Zak�ada si� rysowanie i wype�nianie z poziomu aplikacji z u�yciem pe�nych kolor�w. Ditchering i szrafy mog� zosta� zastosowane przez implementacje SYMSHELL'a w wypadku niedost�pno�ci na urz�dzeniu wyj�ciowym 256 kolor�w.
Parametr "c" oznacza zawsze indeks koloru, parametry "x*","y*" oznaczaj� wsp�rz�dne ekranowe, parametr "r" promie� okr�gu.
void plot(int x, int y, unsigned char c); 
Wy�wietlenie punktu na ekranie.
void line(int x1,int y1,int x2,int y2,unsigned char c);
Wy�wietlenie linii od punktu x1y1 do punktu x2y2 .
void circle(int x, int y, int r, unsigned char c);
Wy�wietlenie okr�gu o promieniu r w kolorze c, o �rodku w punkcie xy oczywi�cie.
void fill_circle(int x, int y, int r, unsigned char c);
Wy�wietla ko�o (pe�ne) o promieniu r w kolorze c .
void fill_rect(int x1,int y1, int x2,int y2, unsigned char c);
Wy�wietla prostok�t wype�niony kolorem "c" o lewym g�rnym rogu w punkcie "x1 y1", a prawym dolnym w punkcie "x2 y2".
void fill_poly(int vx,int vy,
		const ssh_point points[], int 	number,
		unsigned char c);
Wype�nia kolorem "c" wielok�t o "number" wierzcho�k�w zdefiniowanych tablic� punkt�w "points" i przesuni�ty o wektor "vx","vy" w stosunku do �rodka uk�adu wsp�rz�dnych ekranowych. W obecnej wersji dla biblioteki Curses wielok�t nie jest wype�niany!
void clear_screen();
Czy�ci ekran wype�niaj�c go kolorem ustalonym funkcj� set_background() .
Operacje na kolorach:
void set_rgb(int color, int r, int g, int b);
Zmienia definicje sk�adowych koloru. Aktualna implementacja przyjmuje indeksy kolor�w w zakresie 0..255 . Wi�ksza ilo�� kolor�w mog�aby by� nieprzeno�na. Standardowo SYMSHELL inicjuje sk�adowe kolor�w w ci�g�� ska�� barwn� przypominaj�c� skal� temperaturow�. Mo�liwe s� jednak dowolne zmiany o ile umo�liwia je platforma.
Parametr "color" oznacza indeks koloru, kt�rego sk�adowe RGB maj� by� zmienione. Pozosta�e parametry r,b,g s�u�� do podania warto�ci w zakresach od 0 do 255. Nie nale�y raczej zmienia� definicji koloru o indeksie 0 (BLACK:RGB=0.0.0) oraz o indeksie 255 (WHITE: RGB=255.255.255). W wypadku braku kolor�w na urz�dzeniu wyj�ciowym implementacja SYMSHELL'a stara si� je zasymulowa�, mo�e jednak nie by� w stanie zmieni� definicji kolor�w albo wykorzysta� pe�nej informacji. Np na karcie VGA sk�adowe maj� zakresy 6-cio a nie 8-mio bitowe. Obecnie symulacji kolor�w dokonuje wersja dla platformy CURSES, a wersja dla Win32 robi to de facto na bazie domy�lnego=systemowego ditheringu kolor�w w Windows.
�
U�YCIE SYMSHELL'A W APLIKACJACH
Poniewa� zestaw funkcji SYMSHELL'a s� jest stosunkowo nieliczny s� one zestawione w pojedyncze pliki �r�d�owe. Ogranicza to za�miecanie przestrzeni nazw aplikacji pomocniczymi zmiennymi implementacji SYMSHELL'a i u�atwia ich u�ycie, kt�re polega na umieszczeniu plik�w nag��wkowych pakietu w miejscu widocznym dla kompilatora C/C++ i odpowiedniego pliku symsh*.c lub symsh*.o* w zestawie plik�w projektu. Je�li aplikacja nie definiuje w kt�rym� ze swoich w�asnych plik�w zmiennej WB_error_enter_before_clean nale�y do��czy� tak�e plik errorent.c .
	W przypadku �rodowiska X-windows nale�y tak�e do��czy� do projektu plik wbx11ico.c lub w�asny zgodny z plikiem icon.h . Zawiera on dane ikony wy�wietlanej w zast�pstwie zminimalizowanej aplikacji. Prawie dowolny typ grafiki binarnej mo�na przekszta�ci� na format tekstowy za pomoc� programy XV dost�pnego na zasadach licencji GNU na praktycznie ka�d� platform� UNIX'ow�.
	W przypadku stosowania wersji symshwin.c zadanie jest znacznie bardziej skomplikowane ze wzgl�du na specyficzny i nieprzeno�ny charakter aplikacji dla okienkowych system�w Microsoftu. Poza do��czeniem pliku symshwin.c lub symshwin.obj do projektu aplikacji konieczne jest tak�e zdefiniowanie odpowiednich zasob�w aplikacji. Pakiet zawiera wzorcowy plik zasob�w o nazwie symshwin.rc lub analogiczny przystosowany do kreatora zasob�w z MS DEVELOPER STUDIO o nazwie symshmsc.rc. Do��czenie tego lub analogicznego (patrz dalej) pliku zasob�w do projektu jest niezb�dne, �eby aplikacja u�ywaj�ca SYMSHELL'a dla Windows w og�le otworzy�a okno graficzne! 
	Zalecany spos�b modyfikacji zasob�w to skopiowanie odpowiedniego wzorcowego pliku *.rc oraz pliku resources.h do katalogu zawieraj�cego pliki �r�d�owe aplikacji. Zale�nie od kompilatora mo�e te� okaza� si� konieczne wskazanie �cie�ki do pozosta�ych plik�w nag��wkowych SYMSHELL'a w opcjach narz�dzia do modyfikacji zasob�w. Nast�pnie nale�y sprawdzi� czy aplikacja jest budowana i uruchamiana ze standardowym plikiem zasob�w. Po przebrni�ciu z sukcesem przez ten etap mo�na przyst�pi� do modyfikacji pliku zasob�w na potrzeby konkretnej aplikacji. Mo�na zmienia� nast�puj�ce zasoby:
Akcelerator - dowolnie, pod warunkiem �e nazwa zasobu b�dzie zgodna z nazw� aplikacji podan� w �a�cuchu IDS_APPNAME.
Ikona - dowolnie, pod warunkiem, �e b�dzie zdefiniowana ikona 16x16 i 32x32, a tak�e, �e nie zostanie zmieniony identyfikator ikony aplikacji - IDI_APPICON.
Menu - dowolnie, pod warunkiem �e nazwa zasobu b�dzie zgodna z nazw� aplikacji podan� w �a�cuchu IDS_APPNAME. Identyfikatory wybranych pozycji s� przekazywane do aplikacji za po�rednictwem funkcji get_char(), nale�y wi�c uwa�a� by by�y wi�ksze od 255. Kreator zasob�w z MS Visual C++ generuje te warto�ci poczynaj�c od liczby 40004.
Tablica �a�cuch�w - mo�e by� dowolnie rozbudowywana, ale przeno�na aplikacja SYMSHELL'a mo�e i musi skorzysta� jedynie z dw�ch warto�ci: ISD_APPNAME oraz IDS_DESCRIPTION.
Informacja o wersji - nale�y wype�ni� danymi aplikacji. Sam SYMSHELL na razie nie u�ywa tych danych, niewykluczone jednak �e robi� to narz�dzia systemowe.
�
OBECNY STATUS I PERSPEKTYWY ROZWOJU
	SYMSHELL zosta� zaprojektowany pocz�tkowo na potrzeby aplikacji pisanych przez autora w Instytucie Studi�w Spo�ecznych i Instytucie Botaniki Uniwersytetu Warszawskiego.
Obecnie za jego pomoc� utworzono ju� kilka aplikacji, kt�re ci�gle s� rozwijane, a wraz z nimi b�d� te� rozwijane implementacje SYMSHELL'a. Pakiet zostanie te� wykorzystany jako narz�dzie pomocnicze w Warsztatach z Programowania Modeli, kt�re po raz pierwszy odb�d� si� w Instytucie Studi�w Spo�ecznych w semestrze letnim 1997. B�dzie te� udost�pniony magistrantom Zak�adu Systematyki Ro�lin realizuj�cym prace o profilu "komputerowy".
Poza tym planowane jest udost�pnienie publiczne poprzez Internet wersji demo i taniej wersji shareware dla cel�w edukacyjnych i naukowych. Niezale�nie od tego ka�da polskoj�zyczna (na razie - ze wzgl�du na dokumentacj�) osoba fizyczna mo�e pertraktowa� z autorem na temat zakupu pe�nej imiennej wersji zawieraj�cej kod �r�d�owy.
Kontakt z autorem mo�na uzyska� pod nast�puj�cymi adresami:
	borkowsk@bot.strouw.edu.pl 
	borkowsk@samba.iss.uw.edu.pl
