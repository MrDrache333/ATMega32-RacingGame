//----------------------------------------------------------------------
// Titel     : ATMegaRacer
//----------------------------------------------------------------------
// Funktion  : Kleines ATMega Racer spiel
// Schaltung : ATMega32
//----------------------------------------------------------------------
// Prozessor : ATMega32
// Takt		 : 16 MHz
// Sprache   : C
// Datum     : 21.12.2017
// Version   : 1.6a
// Autor     : Keno Oelrichs Garcia
//----------------------------------------------------------------------
/*
	Changelog:
	V1.0a - 21.12.2017
		- Funktionierendes Spiel (Nur Waende ausweichen)
		- Highscore wird oben Links angezeigt
	V1.5a - 10.01.2018
		- Alle 25 Punkte wechselt die Richtung (Alle Waende wechseln durch Animation die Seite)
		- Code Optimierungen
	V1.6a - 11.01.2018
		- Funktionen zum neuschreiben des Displays wurden verbessert
		- Animationen sind fluessiger
		- Waende blinken einmal vor dem Despawnen

*/
#define 	F_CPU 16000000	// Taktfrequenz des myAVR-Boards
#include	<avr\io.h>		// AVR Register und Konstantendefinitionen
#include	<avr\functions_keno.h>	//Meine eigene Funktionssammlung einbinden
#include	<stdlib.h>	//Fuer die Random Funktion
//----------------------------------------------------------------------
//Taster Pins und Port definieren
#define BTNPORT DDRA
#define BTN1 PA5
#define BTN2 PA6
#define BTN3 PA7
#define SEK 49910	//Timer Dauer fuer 1 sek.
#define TIMERMAX 65535	//Timer max. Wert

//GAME
const unsigned int DL = 20;	//Spalten
const unsigned int DH = 4;	//Zeilen
const unsigned int DBO = 5;	//Abstand zwischen Hindernissen
const unsigned int OBS = 5;	//Groesse des Hinderniss Arrays
const unsigned int SPEED = 50;	//IN MS (50-1000)
const unsigned int GAMECURSORSPEED = 70;	//Delay des Cursors im Game in ms 70
const unsigned char SHIP = 0x15;
const unsigned char SHIPR = 0x14;
const unsigned char WALL = 0x1f;
char SHIPMOVESPEED = 1;
char WALLMOVESPEED = -1;
bool GOTODEST = false;

//MENUE
const unsigned int MENUECURSORSPEED = 250;	//Delay des Cursors im Menue in ms
uint8_t SHOWEDMENUE = 1;	//Aktuell angewahltes Menue
const unsigned int ANZMENUES = 3;	//Max Anzahl der Menues 

//Fuktionsdefinitionen (damit Funktionschaos moeglich ist :) )
void gameOver();
void displayStartup();
void timerInit();
void gameTimerInit();
void switchDirect();

//Wand Objekt (Hindernis)
typedef struct {
	uint8_t x;	//Position
	uint8_t dest;
	uint8_t lastX;
	bool walls[DH];	//Wand
	bool active;	//Wand Aktiv bzw. Sichtbar
	uint8_t item;
	
	void init(){	//Initialisieren
		x = WALLMOVESPEED < 0 ? DL:1;
		dest = x;
		lastX = x;
		//Wand erstellen 
		for(uint8_t i = 0; i < DH; i++){
			walls[i] = false;
		}
		active = false;
	}
	
	void move(){	//Wand um 1 stelle verschieben
		lastX = x;
		if (GOTODEST){
			if (x != dest){
				if (x < dest)x+= WALLMOVESPEED < 0 ? (WALLMOVESPEED*-1):WALLMOVESPEED;
				else
					x+= WALLMOVESPEED > 0 ? (WALLMOVESPEED*-1):WALLMOVESPEED;
			}
		}else{
			x+= WALLMOVESPEED;
			dest = x;
		}
	}
	
	void create(){	//Wand erstellen
		//Wand kreieren (Alle Aktiv und einen zufaellig deaktivieren)
		for(uint8_t i = 0; i < DH;i++){
			walls[i] = true;
		}
		walls[rand() % DH] = false;
		
		uint8_t r_gab = rand() % 20;
		if (r_gab == 1){
			walls[rand() % DH] = false;
			
		}else
			item = 0;
		
		x = 0;
		active = false;
	}
	
	void show(){	//Wand anzeigen
		//Alle Zeilen durchschleifen
		for(uint8_t i = 0; i < DH;i++){
			if ((WALLMOVESPEED < 0 ? 18:x) > (WALLMOVESPEED < 0 ? x:3) || i != 0){
				if (walls[i]){	//Wenn an Position Wand erstellt werden soll
					lcdWriteAt_420(1+i,x,WALL);
				}else{
					lcdWriteAt_420(1+i,x," ");
				}
			}
			//Wenn die vorherige Position geloescht werden kann
			//if ((x < DL && WALLMOVESPEED < 0) || (x > 0 && WALLMOVESPEED > 0))lcdWriteAt_420(1+i,x+(-1 * WALLMOVESPEED)," ");
			if (lastX != x)lcdWriteAt_420(1+i,lastX," ");
		}
	}
	//Wand an Startposition anzeigen
	void spawn(){
		x = WALLMOVESPEED < 0 ? DL:1;
		show();
		active = true;
	}
	//Position der Wand ueberpruefen (wenn "offscreen", Wand deaktivieren und loeschen)
	void checkPos(){
		if ((x <= 1 && WALLMOVESPEED < 0) || (x >= DL && WALLMOVESPEED > 0)){
		active = false;
			for(uint8_t i = 0; i < DH;i++){
				if(WALLMOVESPEED < 0)lcdWriteAt_420(1+i,1," "); else lcdWriteAt_420(1+i,DL," ");
			}
		}
	}
	
	bool atDest(){
		return x==dest;
	}
	
	void setDest(uint8_t Dest){
		dest = Dest;
	}
	
	//Getter und Setter
	
	uint8_t getX(){
		return x;
	}
	
	uint8_t getDest(){
		return dest;
	}
	
	bool isActive(){
		return active;
	}
	
	bool getWall(uint8_t i){
		return walls[i - 1];
	}
	
	bool getWalls(){
		return walls;
	}
}Obstacle;

//Spieler-Schiff Objekt
typedef struct{
	uint8_t x;	//Position
	uint8_t lastX;
	uint8_t dest;
	uint8_t y;	//Position
	unsigned int points;	//Punkte
	
	//Schiff erstellen
	void create(){
		x = WALLMOVESPEED < 0 ? 5:DL-4;
		dest = x;
		lastX = x;
		y = DH / 2;
		points = 0;
	}
	
	//Punkte hochzaehlen
	void addPoint(){
		points++;
		if (!(points % 25)){
			if (!GOTODEST)switchDirect();
		}

	}
	//Punkte auf Display anzeigen
	void showPoints(){
		lcdGoto_420(1,WALLMOVESPEED < 0 ? 18:1);
		lcdPrintFloat(points,3,0,0,0);
	}
	//Auf Kollision zwischen einer Wand und dem Schiff pruefen (X und Y vergleichen)
	bool checkCollision(Obstacle object){
		return (object.getX() == x && object.getWall(y));
	}
	//Schiff auf aktueller Position anzeigen
	void show(){
		if (WALLMOVESPEED < 0)lcdWriteAt_420(y,x,SHIP); else lcdWriteAt_420(y,x,SHIPR);
		if (lastX != x)lcdWriteAt_420(y,lastX," ");
	}
	
	//Schiff an finale Position bewegen
	void move(){
		if (x != dest){
			lastX = x;
			if (x < dest) x+=1; else x-=1;
		}
	
	}
	
	//Schiff um 1 nach Oben setzen und vorherige Position loeschen
	void goUp(){
		if (y >= 2){
			cli();
			y--;
			show();
			lcdWriteAt_420(y + 1,x,' ');
			sei();
		}
	}
	//Schiff um 1 nach unten setzen und vorherige Position loeschen
	void goDown(){
		if (y < DH){
			cli();
			y++;
			show();
			lcdWriteAt_420(y - 1,x,' ');
			sei();
		}
	}
		
	bool atDest(){
		return x==dest;
	}
	
	void setDest(uint8_t Dest){
		dest = Dest;
	}
	
	//Getter und Setter
	
	uint8_t getX(){
		return x;
	}
	
	uint8_t getY(){
		return y;
	}
	
	uint8_t getDest(){
		return dest;
	}

}Ship;

//Variablen 
bool gameStarted = false;	//Spiel gestartet
bool gameIsOver = false;	//Spiel ist Verloren
int blinkCounter = 0;	//Speichern von verschiedenen Countern
bool blinkShowMode = false;	//Variable fuer Blinkfunktionen
Obstacle obs[OBS];	//Wandobjekte
Ship player;	//Spieler Objekt
unsigned int CursorSpeed = MENUECURSORSPEED;	//Geschwindigkeit des Cursors

//Spiel vorbei
void gameOver(){
	gameIsOver = true;
	timerInit();	//Timer auf normale geschwindigkeit zuruecksetzen
	lcdWriteAt_420(2,6,"!GAME OVER!");	//Nachricht ausgben
	CursorSpeed = MENUECURSORSPEED;	//Cursorgeschwindigkeit aendern


}
//Normalen Timer initialisieren
void timerInit(){
	//Timer starten
	timer1Init(1024,TIMERMAX - (TIMERMAX - SEK) / (1000 / 500));
}
//Game Timer initialisieren
void gameTimerInit(){
	//Timer starten
	timer1Init(1024,TIMERMAX - (TIMERMAX - SEK) / (1000 / SPEED));
}
//Menueauswahlsymbole aktivieren/deaktivieren
void menueSet(bool active){
	if (active){
		lcdWriteAt_420(SHOWEDMENUE + 1,2,">");
		lcdWriteAt_420(SHOWEDMENUE + 1,19,"<");
	}else{
		lcdWriteAt_420(SHOWEDMENUE + 1,2," ");
		lcdWriteAt_420(SHOWEDMENUE + 1,19," ");
	}
}
//Wechsel der Menueauswahlsymbole durchfuehren
void toggleBedienung(){
	menueSet(blinkShowMode);
	blinkShowMode = !blinkShowMode;
}

void switchDirect(){
		cli();
		for(uint8_t i = 0; i < OBS; i++){
			obs[i].setDest(DL - obs[i].getX());
		}
		player.setDest(DL-player.getX());
		
		WALLMOVESPEED/= -1;
		GOTODEST = true;
		sei();
}

//Spiel mit initialisierung starten
void startGame(){
	if (!gameStarted){	//nur initialisieren wenn Spiel noch nicht gestartet
		gameStarted = true;
		gameTimerInit();	//Spieltimer starten
		CursorSpeed = GAMECURSORSPEED;	//Cursorspeed aendern
		blinkCounter = 0;
		lcdClear();
		player.create();	//Spieler erstellen
		
		//Erstinitialisierung der Hindernisse
		for(uint8_t i = 0; i < OBS; i++){
			obs[i].init();
		}
		//Erstes Hindernis erstellen um den automatischen Ablauf zu aktivieren
		cli();
		obs[0].create();
		obs[0].spawn();
		_delay_ms(100);
		player.show();	//Spieler anzeigen
		sei();
	}
}

//Interrupt bei Timer Overflow
ISR(TIMER1_OVF_vect){
	cli();
	if(gameStarted){	//Wenn Spiel gestartet
		if(!gameIsOver){	//Wenn Spiel nicht Verloren
			bool newGOTODEST = GOTODEST;
			if (GOTODEST){
				//lcdClear();
				player.move();
				newGOTODEST &= player.atDest();
			}
			for(uint8_t i = 0; i < OBS; i++){	//Hindernisse durchschleifen
				obs[i].checkPos();	//Position des Hindernisses 
				if(obs[i].isActive()){	//Wenn Objekt aktuell aktiv ist
					obs[i].move();	//Objekt bewegen
					if (GOTODEST){
					 	newGOTODEST &= obs[i].atDest();
					}
					obs[i].show();	//Objekt anzeigen
					if(obs[i].getX() == player.getX() && !GOTODEST){	//Wenn Objekt und Spieler auf gleicher Position
						if(player.checkCollision(obs[i])){	//Wenn Spieler Objekt beruehrt
							gameOver();	//Spiel Verloren
							break;	//Schleife verlassen
						}else
							player.addPoint();	//Wenn Spieler einer Wand ausgewichen -> Punkt
					}
				}
			}
			if (GOTODEST){
				GOTODEST = !newGOTODEST;
				blinkCounter = 0;
			}
			if(blinkCounter >= DBO && !GOTODEST){	//Spawnen der Waende (Alle X Timeraufrufe)
				blinkCounter = 0;
				for(uint8_t i = 0; i < OBS; i++){	//Objekte durchschleifen
					if (!obs[i].isActive()){	//Wenn Objekt nicht aktiv ist
						obs[i].create();	//Objekt erstellen
						obs[i].spawn();	//Objekt an startpukt anzeigen
						break;	//Schleife verlassen
					}
				}
			}else{
				blinkCounter++;	//Wenn keine Wand spawnen -> Zaehler hochzaehlen
			}
			player.show();	//Spieler anzeigen
			player.showPoints();	//Punkte des Spielers anzeigen
		}else{
			//GAMEOVER
			if(blinkShowMode){	//Wenn Wechselspeicher
				lcdWriteAt_420(player.getY(),player.getX(),WALLMOVESPEED < 0 ? SHIP:SHIPR);	//Schiff anzeigen
			}else{
				lcdWriteAt_420(player.getY(),player.getX(),WALL);	//Wand anzeigen
			}
			blinkShowMode = !blinkShowMode;	//Wechselspeicher wechseln
		}
	}else{
		toggleBedienung();	//Blinken der Menueelement auswahl
	}
	timer1_ResetValue();	//Timer Zahlregister zuruecksetzen
	sei();	//Interrupts erlauben
}
//Wenn Taster 1 gedrueckt wurde
void btn1_pressed(){
	if (!gameStarted){	//Wenn Spiel noch nicht gestartet
		menueSet(false);	//Menuesymbole an aktiver Position loeschen
		if (SHOWEDMENUE > 1)SHOWEDMENUE--;	//Menuesymbol verschieben
		menueSet(true);	//menuesymbol anzeigen
	}else if(!gameIsOver && !GOTODEST){	//Wenn das Spiel angefangen und noch nicht verloren
		player.goUp();	//Spielerposition hoch
	}
}

//Wenn Taster 2 gedrueckt wurde
void btn2_pressed(){
	if (!gameStarted){	//Wenn Spiel noch nicht gestartet
		menueSet(false);	//Menuesymbole an aktiver Position loeschen
		if (SHOWEDMENUE < ANZMENUES)SHOWEDMENUE++;	//Menuesymbol verschieben
		menueSet(true);	//menuesymbol anzeigen
	}else if(!gameIsOver && !GOTODEST){	//Wenn das Spiel angefangen und noch nicht verloren
		player.goDown();	//Spielerposition hoch
	}
}

//Wenn Taster 3 gedrueckt wurde
void btn3_pressed(){
	if (!gameStarted){	//Wenn Spiel noch nicht gestartet
		switch(SHOWEDMENUE){	//Aktuelles Menue ermitteln
			case 1:{	//Spiel starten
				startGame();
				break;
			}
			case 2:{	//Einstellungen
			
				break;
			}
			case 3:{	//Hilfe
			
				break;
			}
			default:{break;}
		}
	}else if (gameIsOver){	//Wenn Spiel gestartet aber Verloren
		gameStarted = false;
		gameIsOver = false;
		displayStartup();	//Haumptmenue anzeigen
	}else{
	}
}


//Startnachricht ausgeben
void displayStartup(){
	//Startinfo
	//Informationen auf dem Display ausgeben
	cli();
	lcdClear();
	lcdWriteAt_420(1,1,"===MEGARACER V1.6a==");
	lcdWriteAt_420(2,1,"   Spiel starten    ");
	lcdWriteAt_420(3,1,"   Einstellungen    ");
	lcdWriteAt_420(4,1,"       Hilfe        ");
	sei();
}
//Bedienung anzeigen
void showBedienung(){
	lcdClear();
	lcdWriteAt_420(1,1,"     BEDIENUNG      ");
	lcdWriteAt_420(2,1,"   Tastenbelegung   ");
	lcdWriteAt_420(3,1,"   des Einschubs    ");
	lcdWriteAt_420(4,1," HOCH  RUNTER   OK  ");
	_delay_ms(3000);
}

//Grundlegende Initialisierungen vornehmen
void init(){
	
	//Eingaenge definieren
	pinMode(BTNPORT, BTN1, INPUT);
	pinMode(BTNPORT, BTN2, INPUT);
	pinMode(BTNPORT, BTN3, INPUT);
	//Pullups setzen
	PORTA |= (1<<BTN1 | 1<<BTN2 | 1<<BTN3);
	//LCD Initialisieren und Loeschen
	lcdInit();
	lcdClear();
	
	//Cursor ausblenden und Startbildschirm anzeigen, Timer Initialisieren
	lcdDisableCursor();
	showBedienung();
	displayStartup();
	timerInit();
}

main (){
	//Initialisierungen vornehmen und Interrupts erlauben
	init();
	sei();
	do {
		//Taster abfragen (Ohne Interrupt da dadurch fehler passiert sind)
		if (!digitalRead(PINA,BTN1)){btn1_pressed();_delay_ms(CursorSpeed);}
		if (!digitalRead(PINA,BTN2)){btn2_pressed();_delay_ms(CursorSpeed);}
		if (!digitalRead(PINA,BTN3)){btn3_pressed();_delay_ms(CursorSpeed);}
	} while (true);
}