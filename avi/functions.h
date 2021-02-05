//--------------------------------------------------------------------------------
// Titel : Gebraeuchliche Funktionen fuer ATMega32, LCD und UART
//--------------------------------------------------------------------------------
// Beschreibung:
//
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
// Sprache : C
// Stand : 15.1.2018
// Version : 1.9.6.2
// Autor : Keno Oelrichs Garcia
// Programmer:
// Port :
//--------------------------------------------------------------------------------
#include <avr\interrupt.h>
#include <util\delay.h>
#include <math.h>

#ifndef BAUD
/* gegen Compiler error bei nicht definierung */
# warning "BAUD not defined for uartInit() set it to 9600(DEFAULT)"
# define BAUD 9600
#endif

//Zustaende
#define HIGH 1
#define LOW 0
#define INPUT 0
#define OUTPUT 1
#define ON 0
#define OFF 1

//Zustaende definieren die das Auswaehlen der Interruptart vereinfachen
#define LOW 0
#define RISING 1
#define FALLING 2
#define CHANGE 3
#define ADC0 0
#define ADC1 1
#define ADC2 2
#define ADC3 3
#define ADC4 4
#define ADC5 5
#define ADC6 6
#define ADC7 7



volatile static uint16_t TIMER1_CV;	//Vergleichswert fuer Timer1

//Zuruecksetzen des Vergleichs_wertes
void timer1_ResetValue(){
	TCNT1 = TIMER1_CV;
}

//Initialisieren des Timers
void timer1Init(uint16_t VORTEILER, uint16_t VERGLEICHSWERT){
	TIMER1_CV = VERGLEICHSWERT;
	//Timer 1 initialisieren
 	switch(VORTEILER){
 		case 1:{
 			TCCR1B = 1;
 		break;
 		}
 		case 8:{
 			TCCR1B = 2;
 		break;
 		}
 		case 64:{
 			TCCR1B = 3;
 		break;
 		}
 		case 256:{
 			TCCR1B = 4;
 		break;
 		}
 		case 1024:{
 			TCCR1B = 5;
 		break;
 		}
 		default:{
 			TCCR1B = 1;
 		break;
 		}
 	}
 	TIMSK=0x04; // Interrupt bei Vergleichswert A
 	TCNT1 = TIMER1_CV;
}

// myWait_ms - Warte-Routine fuer x-Millisekunden (0-255)
// ein Millisekundenzyklus dauert 997,5 �s
// PE: Anzahl der zu wartenden Milisekunden
//------------------------------------------------------------------------
void myWait_ms(uint8_t dauer_ms){
 	__asm__ volatile (
 	"myWait_ms_0%=:\n"
 	"push 	%0\n"
 	"ldi 	%0,1\n"
 	"myWait_ms_3%=:\n"
 	"push 	%0\n"
 	"ldi 	%0,62\n"
 	"myWait_ms_2%=:\n"
 	"push 	%0\n"
 	"ldi 	%0,255\n"
 	"myWait_ms_1%=:\n"
 	"dec 	%0 	\n"
 	"brne 	myWait_ms_1%=\n"
 	"pop 	%0 	; Register wiederherstellen\n"
 	"dec 	%0 	\n"
 	"brne 	myWait_ms_2%=\n"
 	"pop 	%0 	; Register wiederherstellen\n"
 	"dec 	%0 	\n"
 	"brne 	myWait_ms_3%=\n"
 	"pop 	%0 	; Register wiederherstellen\n"
 	"dec 	%0 	\n"
 	"brne 	myWait_ms_0%=\n"
 	:
 	:"d" (dauer_ms)
 	);
}

//--------------------------------------------------------------------
// myWait_us - Warte-Routine fuer x-Mikrosekunden einschlie�lich des Aufrufes (1-255)
// berechnete Abweichung: 0%
// PE: dauer_us = Anzahl der zu wartenden Mikrosekunden
// es wird mindestens 1�s gewartet, auch wenn r16 kleiner als 1
//--------------------------------------------------------------------
void myWait_us(uint8_t dauer_us){
 	__asm__ volatile (
 	"push r16\n"
 	"pop r16\n"
 	"nop 	; wait\n"
 	"subi 	%0,1\n"
 	"brcs 	myWait_us_1%=\n"
 	"breq 	myWait_us_1%=\n"
 	"myWait_us_2%=:\n"
 	"push r16\n"
 	"pop r16\n"
 	"push r16\n"
 	"pop r16\n"
 	"push r16\n"
 	"pop r16\n"
 	"nop 	; wait\n"
 	"dec 	%0\n"
 	"brne 	myWait_us_2%=\n"
 	"myWait_us_1%=:\n"
 	:
 	:"d" (dauer_us)
 	);
}

//------------------------------------------------------------------------
// Ausgabe LTC1446 24Bit Seriell
//------------------------------------------------------------------------
void LTC1446(int Vout_A,int Vout_B){
    int data[24]={0};	// Array erzeugen fuer beide DA-Werte
    
    // Zerlegen der Zahl"VoutA" in 12 einzellne Bits, MSB zuerst
    
    while(Vout_A>=2048)
    {
        data[1]=1;
        Vout_A+=-2048;
    }
    while(Vout_A>=1024)
    {
        data[2]=1;
        Vout_A+=-1024;
    }
    while(Vout_A>=512)
    {
        data[3]=1;
        Vout_A+=-512;
    }
    while(Vout_A>=256)
    {
        data[4]=1;
        Vout_A+=-256;
    }
    while(Vout_A>=128)
    {
        data[5]=1;
        Vout_A+=-128;
    }
    while(Vout_A>=64)
    {
        data[6]=1;
        Vout_A+=-64;
    }
    while(Vout_A>=32)
    {
        data[7]=1;
        Vout_A+=-32;
    }
    while(Vout_A>=16)
    {
        data[8]=1;
        Vout_A+=-16;
    }
    while(Vout_A>=8)
    {
        data[9]=1;
        Vout_A+=-8;
    }
    while(Vout_A>=4)
    {
        data[10]=1;
        Vout_A+=-4;
    }
    while(Vout_A>=2)
    {
        data[11]=1;
        Vout_A+=-2;
    }
    
    if (Vout_A == 1)
        data[12] = 1;
    else
        data[12] = 0;
    
    // Zerlegen der Zahl"VoutB" in 12 einzellne Bits, MSB zuerst
    
    while(Vout_B>=2048)
    {
        data[13]=1;
        Vout_B+=-2048;
    }
    while(Vout_B>=1024)
    {
        data[14]=1;
        Vout_B+=-1024;
    }
    while(Vout_B>=512)
    {
        data[15]=1;
        Vout_B+=-512;
    }
    while(Vout_B>=256)
    {
        data[16]=1;
        Vout_B+=-256;
    }
    while(Vout_B>=128)
    {
        data[17]=1;
        Vout_B+=-128;
    }
    while(Vout_B>=64)
    {
        data[18]=1;
        Vout_B+=-64;
    }
    while(Vout_B>=32)
    {
        data[19]=1;
        Vout_B+=-32;
    }
    while(Vout_B>=16)
    {
        data[20]=1;
        Vout_B+=-16;
    }
    while(Vout_B>=8)
    {
        data[21]=1;
        Vout_B+=-8;
    }
    while(Vout_B>=4)
    {
        data[22]=1;
        Vout_B+=-4;
    }
    while(Vout_B>=2)
    {
        data[23]=1;
        Vout_B+=-2;
    }
    
    if (Vout_B == 1)
        data[24] = 1;
    else
        data[24] = 0;
    
    // Aufbau und Ausgabe des Datenwortes mit 3 Signalen an PORT C0 - C2
    // C0 = Data, C1 = CSN, C2 = CLK
    for (int i =  1; i < 25; i++)				// 24 Schritte beginnend mit 1
    {
        if (i == 24)							// CSN auf 1 in CLK 24
        {
            PORTC = PORTC & 0b11111011;			// CLK auf 0
            myWait_us(2);
            if(data[i] == 1)
            {
                PORTC = PORTC | 0b00000001;		// Data auf 1
            }
            else
            {
                PORTC = PORTC & 0b11111110;		// Data auf 0
            }
            myWait_us(3);
            PORTC = PORTC | 0b00000100;			// CLK auf 1
            
            myWait_us(5);
            PORTC = PORTC | 0b00000010;			// CSN auf 1
            myWait_us(5);
            
        }
        else
        {
            PORTC = PORTC & 0b11111011;			// CLK auf 0
            myWait_us(1);
            PORTC = PORTC & 0b11111101; 		//Setzt CSN auf 0
            myWait_us(1);
            
            if(data[i] == 1)
            {
                PORTC = PORTC | 0b00000001;		// Data auf 1
            }
            else
            {
                PORTC = PORTC & 0b11111110;		// Data auf 0
            }
            myWait_us(3);	
            PORTC = PORTC | 0b00000100;			// CLK auf 1				
            myWait_us(5);											
        }
    }
    myWait_us(15);
}

//////////////////////////////////////////////////////////////////////////////
// 	LCD-Funktionen fuer myAVR-Board + myAVR-LCD
// 	4-BitModus an PortD Bit 4-7
// 	PortD Bit 2 = RS, high=Daten, low=Kommando
// 	PortD Bit 3 = E, high-Impuls fuer gueltige Daten
//---------------------------------------------------------------------------
// 	lcdSend(..) - sendet ein Byte an LCD im 4-Bit-Modus
// 	RS muss vorher richtig gesetzt sein
// 	PE: 	data=zu sendendes Byte
//---------------------------------------------------------------------------
void lcdSend(char data){
 	// aktuelles RS ermitteln
 	char rs=PORTD;
 	rs&=4;
 	// High-Teil senden
 	char tmp=data;
 	tmp&=0xf0;
 	tmp|=rs;
 	PORTD=tmp;
 	// Schreibsignal
 	sbi(PORTD,3);
 	cbi(PORTD,3);
 	// Low-Teil senden
 	tmp=data;
 	tmp&=0x0f;
 	tmp*=16;
 	tmp|=rs;
 	PORTD=tmp;
 	// Schreibsignal
 	sbi(PORTD,3);
 	cbi(PORTD,3);
 	// verarbeiten lassen
 	waitMs(1);
}
//---------------------------------------------------------------------------
// 	lcd_write(..) - sendet ein Zeichen (Daten) an LCD
// 	PE: 	text=Zeichen
//---------------------------------------------------------------------------
void lcdWrite(char text){
 	sbi(PORTD,2); 	 	// RS setzen = Daten
 	lcdSend(text); 	 	// senden
}
//---------------------------------------------------------------------------
// 	lcdCmd(..) - sendet ein Kommando an LCD
// 	PE: 	cmd=Kommando-Byte
//---------------------------------------------------------------------------
void lcdCmd(char cmd){
 	cbi(PORTD,2); 	 	// RS loeschen = Kommando
 	lcdSend(cmd); 	 	// senden
}
//---------------------------------------------------------------------------
// 	lcdWrite(..) - sendet eine Zeichenkette an LCD
// 	Die Zeichenkette muss mit 0x00 abgeschlossen sein
// 	PE: 	pText=Zeiger auf Zeichenkette
//---------------------------------------------------------------------------
void lcdWrite(const char* pText){
 	while(pText[0]!=0)
 	{
 	 	lcdWrite(pText[0]);
 	 	pText++;
 	}
}
//---------------------------------------------------------------------------
// 	lcdHome(..) - Cursor auf Position 1,1
//---------------------------------------------------------------------------
void lcdHome(){
 	lcdCmd(0x02);
 	waitMs(2); 	 	 	// warten
}
//---------------------------------------------------------------------------
// 	lcdClear(..) - loescht die Anzeige im LCD
//---------------------------------------------------------------------------
void lcdClear(){
 	lcdCmd(0x01);
 	waitMs(2); 	 	 	// warten
	lcdWrite(" ");
	lcdHome();
}
//---------------------------------------------------------------------------
// 	lcdOn(..) - schaltet das LCD an
//---------------------------------------------------------------------------
void lcdOn(){
 	lcdCmd(0x0E);
}

//---------------------------------------------------------------------------
// 	lcdInit(..) - Schaltet die Ports und Initialisiert das LCD
//---------------------------------------------------------------------------
void lcdInit(){
 	// Port D = Ausgang
 	DDRD=0xff;
 	PORTD=0;
 	// warten bist LCD-Controller gebootet
 	waitMs(50);
 	// SOFT-RESET
 	PORTD = 0x30; 	//0b00110000;
 	sbi(PORTD,3);
 	cbi(PORTD,3);
 	waitMs(5);
 	PORTD = 0x30; 	//0b00110000;
 	sbi(PORTD,3);
 	cbi(PORTD,3);
 	waitMs(1);
 	PORTD = 0x30; 	//0b00110000;
 	sbi(PORTD,3);
 	cbi(PORTD,3);
 	waitMs(5);

 	// 4-BitModus einschalten
 	PORTD=0x20;
 	// Schreibsignal
 	sbi(PORTD,3);
 	cbi(PORTD,3);
 	waitMs(5);

 	// ab hier im 4-Bit-Modus
 	lcdCmd(0x28); 	 	// Funktions-Set: 2 Zeilen, 5x7 Matrix, 4 Bit
 	lcdCmd(0x06); 	 	// Entry Mode
 	lcdOn();
 	lcdClear();
}
//---------------------------------------------------------------------------
// 	lcdWrite(..) - sendet eine Zeichenkette an LCD
// 	PE: 	pText=Zeiger auf Zeichenkette
// 	 	count=Anzahl der zu sendenden Zeichen
//---------------------------------------------------------------------------
void lcdWrite(const char* pText, int count){
 	while(count!=0)
 	{
 	 	lcdWrite(pText[0]);
 	 	pText++;
 	 	count--;
 	}
}
//----------------------------------------------------------------------
// Titel 	: C-Funktion Zeichenkette senden.
//----------------------------------------------------------------------
// Funktion 	: ...
// IN  	 	: char *buffer, Zeichenkette mit NUll abgeschlossen
// OUT 	 	: ...
//----------------------------------------------------------------------
void lcdString(const char *buffer){
 	lcdWrite(buffer);
}
//---------------------------------------------------------------------------
// 	lcdGoto(..) - setzt die Cursorposition
//  	PE: 	row = Zeile 1..2
// 	 	col = Spalte 1..16
//---------------------------------------------------------------------------
void lcdGoto(int row, int col){
 	row--; 	 	 	 	// Null-basierend
 	row&=0x01; 	 	 	// sicherheitshalber
 	row*=0x40; 	 	 	// Zeile nach Bit 6 bringen
 	col--; 	 	 	 	// Null-basierend
 	col&=0x0f; 	 	 	// sicherheitshalber
 	char tmp=row|col;
 	tmp|=0x80; 	 	 	// Cursor setzen
 	lcdCmd(tmp); 	 	// senden
}

//---------------------------------------------------------------------------
// 	lcdGoto_420(row,col) - setzt die Cursorposition
//   	row = Zeile 1..4
// 	 	col = Spalte 1..20
//---------------------------------------------------------------------------
void lcdGoto_420(int row, int col){
		int tmp;
		col--;						// - fuer 0 bis 19
		switch ( row )     				// Reihe
		{
		  case 0x01 : tmp =(0x00 + col);		// 0x00 fuer 1 Zeile + Spalte 
		            break;
		  case 0x02 : tmp =(0x14 + col);		// 0x14 fuer 2 Zeile + Spalte
		            break;
		  case 0x03 : tmp =(0x28 + col);		// 0x28 fuer 3 Zeile + Spalte
		            break;
		  case 0x04 : tmp =(0x54 + col);		// 0x54 fuer 4 Zeile + Spalte
		            break;
		  default :   tmp = 0 ;
		            break;
		}
 
 	tmp|=0x80; 	 	 				// Cursor setzen
 	
 	lcdCmd(tmp); 	 					// senden
}

//---------------------------------------------------------------------------
// 	lcdOff(..) - schaltet das LCD aus
//---------------------------------------------------------------------------
void lcdOff(){
 	lcdCmd(0x08);
}
//---------------------------------------------------------------------------
// lcdZahl(..) - gibt eine 8 Bit Zahl aus
// PE:uint8_t zahl
//---------------------------------------------------------------------------
void lcdZahl(uint16_t zahl){
 	char ziffer1=0;
 	char ziffer2=0;
 	char ziffer3=0;
 	char ziffer4=0;
 	
 	
 	while(zahl>=1000)
 	{
 	 	ziffer1+=1;
 	 	zahl+=-1000;
 	}
 	while(zahl>=100)
 	{
 	 	ziffer2+=1;
 	 	zahl+=-100;
 	}
 	while(zahl>=10)
 	{
 	 	ziffer3+=1;
 	 	zahl+=-10;
 	}

 	ziffer1+=0x30;
 	ziffer2+=0x30;
 	ziffer3+=0x30;
 	ziffer4=zahl+0x30;
 	//lcdWrite(ziffer1);
 	lcdWrite(ziffer2);
 	lcdWrite(ziffer3);
 	lcdWrite(ziffer4);
}


//---------------------------------------------------------------------------
// lcdZahl(..) - gibt eine 8 Bit Zahl aus
// PE:uint8_t zahl
//---------------------------------------------------------------------------
void lcdZahl_1(uint16_t zahl){
 	char ziffer1=0;
 	char ziffer2=0;
 	char ziffer3=0;
 	char ziffer4=0;
 	
 	
 	while(zahl>=1000)
 	{
 	 	ziffer1+=1;
 	 	zahl+=-1000;
 	}
 	while(zahl>=100)
 	{
 	 	ziffer2+=1;
 	 	zahl+=-100;
 	}
 	while(zahl>=10)
 	{
 	 	ziffer3+=1;
 	 	zahl+=-10;
 	}

 	ziffer1+=0x30;
 	ziffer2+=0x30;
 	ziffer3+=0x30;
 	ziffer4=zahl+0x30;
 	lcdWrite(ziffer1);
 	lcdWrite(",");
 	lcdWrite(ziffer2);
 	lcdWrite(ziffer3);
 	//lcdWrite(ziffer4);
}
// lcdZahl(..) - gibt eine 8 Bit Zahl aus
// PE:uint8_t zahl
//---------------------------------------------------------------------------
void lcdZahl_0(uint16_t zahl){
    char ziffer1=0;
    char ziffer2=0;
    char ziffer3=0;
    char ziffer4=0;
    
    
    while(zahl>=1000)
    {
        ziffer1+=1;
        zahl+=-1000;
    }
    while(zahl>=100)
    {
        ziffer2+=1;
        zahl+=-100;
    }
    while(zahl>=10)
    {
        ziffer3+=1;
        zahl+=-10;
    }
    
    ziffer1+=0x30;
    ziffer2+=0x30;
    ziffer3+=0x30;
    ziffer4=zahl+0x30;
    //lcdWrite(ziffer1);
    //lcdWrite(",");
    //lcdWrite(ziffer2);
    //lcdWrite(ziffer3);
    lcdWrite(ziffer4);
}
//--------------------------------------------------------------------
// Gibt eine 4 stellige Zahl im Format XXX,X aus. Eine fuehrende
// Null wird unterdrueckt
//--------------------------------------------------------------------
void lcdZahl_T(uint16_t zahl){
    char ziffer1=0;
    char ziffer2=0;
    char ziffer3=0;
    char ziffer4=0;
    
    
    while(zahl>=1000)
    {
        ziffer1+=1;
        zahl+=-1000;
    }
    while(zahl>=100)
    {
        ziffer2+=1;
        zahl+=-100;
    }
    while(zahl>=10)
    {
        ziffer3+=1;
        zahl+=-10;
    }
    
    ziffer1+=0x30;
    ziffer2+=0x30;
    ziffer3+=0x30;
    ziffer4=zahl+0x30;
    
    if(ziffer1==0x30)
        lcdWrite(" ");
    else
        lcdWrite(ziffer1);
    
    if((ziffer2==0x30)&&(ziffer1==0x30))
        lcdWrite(" ");
    else
        lcdWrite(ziffer2);
    
    
    lcdWrite(ziffer3);
    lcdWrite(",");
    lcdWrite(ziffer4);
}

//--------------------------------------------------------------------

//---------------------------------------------------------------------------
//
// Formatierte ausgabe auf LCD XX,XX
//---------------------------------------------------------------------------
void lcdZahl_2(uint16_t zahl){
    char ziffer1=0;
    char ziffer2=0;
    char ziffer3=0;
    char ziffer4=0;
    
    
    while(zahl>=1000)
    {
        ziffer1+=1;
        zahl+=-1000;
    }
    while(zahl>=100)
    {
        ziffer2+=1;
        zahl+=-100;
    }
    while(zahl>=10)
    {
        ziffer3+=1;
        zahl+=-10;
    }
    
    ziffer1+=0x30;
    ziffer2+=0x30;
    ziffer3+=0x30;
    ziffer4=zahl+0x30;
    lcdWrite(ziffer1);
    lcdWrite(ziffer2);
    lcdWrite(",");
    lcdWrite(ziffer3);
    lcdWrite(ziffer4);
}
//----------------------------------------------------------------------
// Titel 	: C-Funktion Zeichen zu UART senden.
//----------------------------------------------------------------------
// Funktion 	: ...
// IN  	 	: char data
// OUT 	 	: ...
//----------------------------------------------------------------------
void uartPutChar(char data){
    //warte bis UDR leer ist UCSRA / USR bei z.B.: 2313
    while (!(UCSRA&32));
    //sende
    UDR=data;
}
//----------------------------------------------------------------------
// Titel 	: C-Funktion Zeichen von UART holen.
//----------------------------------------------------------------------
// Funktion 	: ...
// IN  	 	: ...
// OUT 	 	: data
//----------------------------------------------------------------------
char uartGetChar(){
 	char data=0;
    //warte bis RX-complete RXC UCSRA / USR bei z.B.: AT090S2313
    while (!(UCSRA&128));
 	//empfangen
    data=UDR;
    	return data;
}
//----------------------------------------------------------------------
// Titel 	: C-Funktion Zeichenkette zu UART senden.
//----------------------------------------------------------------------
// Funktion 	: ...
// IN  	 	: char *buffer, Zeichenkette mit NUll abgeschlossen
// OUT 	 	: ...
//----------------------------------------------------------------------
void uartPutString(const char *buffer){
 	for (int i=0; buffer[i] !=0;i++)
 	 	uartPutChar (buffer[i]);
}


//---------------------------------------------------------------------------
// uartPutZahl Gibt eine 4 Stellige Zahl aus
// PE:uint8_t zahl
//---------------------------------------------------------------------------
void uartPutZahl(uint16_t zahl){
 	char ziffer1=0;
 	char ziffer2=0;
 	char ziffer3=0;
 	char ziffer4=0;
 	
 	
 	while(zahl>=1000)
 	{
 	 	ziffer1+=1;
 	 	zahl+=-1000;
 	}
 	while(zahl>=100)
 	{
 	 	ziffer2+=1;
 	 	zahl+=-100;
 	}
 	while(zahl>=10)
 	{
 	 	ziffer3+=1;
 	 	zahl+=-10;
 	}

 	ziffer1+=0x30;
 	ziffer2+=0x30;
 	ziffer3+=0x30;
 	ziffer4=zahl+0x30;
 	uartPutChar(ziffer1);
 	uartPutChar(ziffer2);
	uartPutChar(ziffer3);
	uartPutChar(ziffer4);
}


void lcdPrintZahl(uint16_t ZAHL, uint8_t KOMMASTELLEN, bool PRINTZERO){
	char ziffer1=0;
 	char ziffer2=0;
 	char ziffer3=0;
 	char ziffer4=0;
	char ziffer5=0;
	
 	while(ZAHL>=10000){
 	 	ziffer1+=1;
 	 	ZAHL+=-10000;
 	}
 	while(ZAHL>=1000){
 	 	ziffer2+=1;
 	 	ZAHL+=-1000;
 	}
 	while(ZAHL>=100)
 	{
 	 	ziffer3+=1;
 	 	ZAHL+=-100;
 	}
 	while(ZAHL>=10)
 	{
 	 	ziffer4+=1;
 	 	ZAHL+=-10;
 	}
	if (!PRINTZERO){
		if (ziffer1 == 0 && KOMMASTELLEN != 4){ziffer1 = 0x20;}else{ ziffer1+=0x30;}
		if (ziffer2 == 0 && ziffer1 == 0x20 && KOMMASTELLEN != 3){ziffer2 = 0x20;}else{ ziffer2+=0x30;}
		if (ziffer3 == 0 && ziffer2 == 0x20 && ziffer1 == 0x20 && KOMMASTELLEN != 2){ziffer3 = 0x20;}else{ ziffer3+=0x30;}
		if (ziffer4 == 0 && ziffer3 == 0 && ziffer2 == 0x20 && ziffer1 == 0x20 && KOMMASTELLEN != 1){ziffer4 = 0x20;}else{ ziffer4+=0x30;}
	 	ziffer5=ZAHL+0x30;
	}else{
	 	ziffer1+=0x30;
 		ziffer2+=0x30;
 		ziffer3+=0x30;
		ziffer4+=0x30;
 		ziffer5=ZAHL+0x30;
	}
	if (KOMMASTELLEN >= 0 && KOMMASTELLEN < 5){
		switch(KOMMASTELLEN){
			case 0:{
				lcdWrite(ziffer1);
 				lcdWrite(ziffer2);
				lcdWrite(ziffer3);
				lcdWrite(ziffer4);
				lcdWrite(ziffer5);
			break;
			}
			
			case 1:{
				lcdWrite(ziffer1);
 				lcdWrite(ziffer2);
				lcdWrite(ziffer3);
				lcdWrite(ziffer4);
				lcdWrite(',');
				lcdWrite(ziffer5);
			break;
			}
			
			case 2:{
				lcdWrite(ziffer1);
 				lcdWrite(ziffer2);
				lcdWrite(ziffer3);
 				lcdWrite(',');
				lcdWrite(ziffer4);
				lcdWrite(ziffer5);
			break;
			}
			
			case 3:{
				lcdWrite(ziffer1);
				lcdWrite(ziffer2);
				lcdWrite(',');
 				lcdWrite(ziffer3);
				lcdWrite(ziffer4);
				lcdWrite(ziffer5);
			break;
			}
			
			case 4:{
				lcdWrite(ziffer1);
				lcdWrite(',');
				lcdWrite(ziffer2);
 				lcdWrite(ziffer3);
				lcdWrite(ziffer4);
				lcdWrite(ziffer5);
			break;
			}
			
			case 5:{
				lcdWrite(0x30);
				lcdWrite(',');
				lcdWrite(ziffer1);
				lcdWrite(ziffer2);
 				lcdWrite(ziffer3);
				lcdWrite(ziffer4);
				lcdWrite(ziffer5);
			break;
			}
			
			default:{break;}
		}
	}
}

void uartPrintZahl(uint16_t ZAHL, uint8_t KOMMASTELLEN, bool PRINTZERO){
	char ziffer1=0;
 	char ziffer2=0;
 	char ziffer3=0;
 	char ziffer4=0;
 	
 	
 	while(ZAHL>=1000)
 	{
 	 	ziffer1+=1;
 	 	ZAHL+=-1000;
 	}
 	while(ZAHL>=100)
 	{
 	 	ziffer2+=1;
 	 	ZAHL+=-100;
 	}
 	while(ZAHL>=10)
 	{
 	 	ziffer3+=1;
 	 	ZAHL+=-10;
 	}
	if (!PRINTZERO){
		if (ziffer1 == 0 && KOMMASTELLEN != 3){ziffer1 = 0x20;}else{ ziffer1+=0x30;}
		if (ziffer2 == 0 && ziffer1 == 0x20 && KOMMASTELLEN != 2){ziffer2 = 0x20;}else{ ziffer2+=0x30;}
		if (ziffer3 == 0 && ziffer2 == 0x20 && ziffer1 == 0x20 && KOMMASTELLEN != 1){ziffer3 = 0x20;}else{ ziffer3+=0x30;}
	 	ziffer4=ZAHL+0x30;
	}else{
	 	ziffer1+=0x30;
 		ziffer2+=0x30;
 		ziffer3+=0x30;
 		ziffer4=ZAHL+0x30;
	}
	if (KOMMASTELLEN >= 0 && KOMMASTELLEN < 5){
		switch(KOMMASTELLEN){
			case 0:{
				uartPutChar(ziffer1);
 				uartPutChar(ziffer2);
				uartPutChar(ziffer3);
				uartPutChar(ziffer4);
			break;
			}
			
			case 1:{
				uartPutChar(ziffer1);
 				uartPutChar(ziffer2);
				uartPutChar(ziffer3);
				uartPutChar(',');
				uartPutChar(ziffer4);
			break;
			}
			
			case 2:{
				uartPutChar(ziffer1);
 				uartPutChar(ziffer2);
 				uartPutChar(',');
				uartPutChar(ziffer3);
				uartPutChar(ziffer4);
			break;
			}
			
			case 3:{
				uartPutChar(ziffer1);
				uartPutChar(',');
 				uartPutChar(ziffer2);
				uartPutChar(ziffer3);
				uartPutChar(ziffer4);
			break;
			}
			
			case 4:{
				uartPutChar(0x30);
				uartPutChar(',');
				uartPutChar(ziffer1);
 				uartPutChar(ziffer2);
				uartPutChar(ziffer3);
				uartPutChar(ziffer4);
			break;
			}
			
			default:{break;}
		}
	}
}

//Die PrintFloat Funktion, welche als ebergabewert eine Kommazahl empfangen kann
//und diese wie mit Parametern uebergeben richtig formatiert und auf Terminal und LCD
//zur Anzeige bringt

void lcdPrintFloat(float ZAHL, uint8_t VORKOMMA, uint8_t NACHKOMMA, bool prefix, bool PRINTZERO){
	//Viel zu viele Variablen
	char ziffer1=0;
 	char ziffer2=0;
 	char ziffer3=0;
 	char ziffer4=0;
 	char ziffer5=0;
 	char ziffer6=0;
 	char ziffer7=0;
	char ziffer8=0;
 	bool negative = false;
	//Pruefen ob Negativ
 	if (ZAHL < 0){
 		ZAHL*=-1;
 		negative = true;
 	}
 	//Stellen zaehlen
	while(ZAHL>=10000)
 	{
 	 	ziffer1+=1;
 	 	ZAHL+=-10000;
 	}
 	while(ZAHL>=1000)
 	{
 	 	ziffer2+=1;
 	 	ZAHL+=-1000;
 	}
 	while(ZAHL>=100)
 	{
 	 	ziffer3+=1;
 	 	ZAHL+=-100;
 	}
 	while(ZAHL>=10)
 	{
 	 	ziffer4+=1;
 	 	ZAHL+=-10;
 	}
 	while(ZAHL>=1){
 		ziffer5++;
 		ZAHL-=1;
 	}
 	while(ZAHL>=0.1){
 		ziffer6++;
 		ZAHL-=0.1;
 	}
 	while(ZAHL>=0.01){
 		ziffer7++;
 		ZAHL-=0.01;
 	}
 	while(ZAHL>=0.001){
 		ziffer8++;
 		ZAHL-=0.001;
 	}
 	
 	//Aufrunden wenn noetig
 	if (NACHKOMMA == 0 && ziffer6 >= 5)ziffer5++;
 	if (NACHKOMMA == 1 && ziffer7 >= 5)ziffer6++;
 	if (NACHKOMMA == 2 && ziffer8 >= 5)ziffer7++;
 	
 	//Zahlenueberlauf bei aufrunden verhindern
	while(ziffer7 > 9){
 	if (ziffer7 > 10)ziffer7--; else ziffer7 = 0;
 	ziffer6++;
 	}
 	while(ziffer6 > 9){
 	if (ziffer6 > 10)ziffer7--; else ziffer6 = 0;
 	ziffer5++;
 	}
 	while(ziffer5 > 9){
 	if (ziffer5 > 10)ziffer7--; else ziffer5 = 0;
 	ziffer4++;
 	}
 	while(ziffer4 > 9){
 	if (ziffer4 > 10)ziffer7--; else ziffer4 = 0;
 	ziffer3++;
 	}
 	while(ziffer3 > 9){
 	if (ziffer3 > 10)ziffer7--; else ziffer3 = 0;
 	ziffer2++;
 	}
 	while(ziffer2 > 9){
 	if (ziffer2 > 10)ziffer7--; else ziffer2 = 0;
 	ziffer1++;
 	}
 	
 	//Wenn 0en nicht ausgegeben werden sollen, durch Leerzeichen ersetzen
	if (!PRINTZERO){
		uint8_t zeroc = 0;
		if (ziffer1 == 0){ziffer1 = 0x20; zeroc++;}else{ ziffer1+=0x30;}
		if (ziffer2 == 0 && zeroc == 1){ziffer2 = 0x20; zeroc++;}else{ ziffer2+=0x30;}
		if (ziffer3 == 0 && zeroc == 2){ziffer3 = 0x20; zeroc++;}else{ ziffer3+=0x30;}
		if (ziffer4 == 0 && zeroc == 3){ziffer4 = 0x20; zeroc++;}else{ ziffer4+=0x30;}
		if (ziffer5 == 0 && zeroc == 4){ziffer5 = 0x30; zeroc++;}else{ ziffer5+=0x30;}
	}else{
	 	ziffer1+=0x30;
 		ziffer2+=0x30;
 		ziffer3+=0x30;
		ziffer4+=0x30;
		ziffer5+=0x30;

	}
	//Die Nachkommastellen benoetigen keine Pruefung auf 0stellen
 	ziffer6+=0x30;
 	ziffer7+=0x30;
	ziffer8+=0x30;
	
	//Vorzeichen ausgeben
	if (prefix){
		if (negative){
			lcdWrite('-');
		}else{
			lcdWrite('+');
		}
	}
	//Anzeigen was angezeigt werden soll
	if (VORKOMMA >= 5)lcdWrite(ziffer1);
	if (VORKOMMA >= 4)lcdWrite(ziffer2);
	if (VORKOMMA >= 3)lcdWrite(ziffer3);
	if (VORKOMMA >= 2)lcdWrite(ziffer4);
	if (VORKOMMA >= 1)lcdWrite(ziffer5);
	if (NACHKOMMA >= 1 || VORKOMMA == 0)lcdWrite(',');
	if (NACHKOMMA >= 1 || VORKOMMA == 0)lcdWrite(ziffer6);
	if (NACHKOMMA >= 2)lcdWrite(ziffer7);
	if (NACHKOMMA >= 3)lcdWrite(ziffer8);
}

void lcdPrintFloat(float ZAHL, uint8_t VORKOMMA, uint8_t NACHKOMMA, bool prefix){
	lcdPrintFloat(ZAHL, VORKOMMA, NACHKOMMA, prefix, 0);
}

void lcdPrintFloat(float ZAHL, uint8_t VORKOMMA, uint8_t NACHKOMMA){
	lcdPrintFloat(ZAHL, VORKOMMA, NACHKOMMA, 1, 0);
}

void uartPrintFloat(float ZAHL, uint8_t VORKOMMA, uint8_t NACHKOMMA, bool prefix, bool PRINTZERO){
	//Viel zu viele Variablen
	char ziffer1=0;
 	char ziffer2=0;
 	char ziffer3=0;
 	char ziffer4=0;
 	char ziffer5=0;
 	char ziffer6=0;
 	char ziffer7=0;
	char ziffer8=0;
 	bool negative = false;
	//Pruefen ob Negativ
 	if (ZAHL < 0){
 		ZAHL*=-1;
 		negative = true;
 	}
 	//Stellen zaehlen
	while(ZAHL>=10000)
 	{
 	 	ziffer1+=1;
 	 	ZAHL+=-10000;
 	}
 	while(ZAHL>=1000)
 	{
 	 	ziffer2+=1;
 	 	ZAHL+=-1000;
 	}
 	while(ZAHL>=100)
 	{
 	 	ziffer3+=1;
 	 	ZAHL+=-100;
 	}
 	while(ZAHL>=10)
 	{
 	 	ziffer4+=1;
 	 	ZAHL+=-10;
 	}
 	while(ZAHL>=1){
 		ziffer5++;
 		ZAHL-=1;
 	}
 	while(ZAHL>=0.1){
 		ziffer6++;
 		ZAHL-=0.1;
 	}
 	while(ZAHL>=0.01){
 		ziffer7++;
 		ZAHL-=0.01;
 	}
 	while(ZAHL>=0.001){
 		ziffer8++;
 		ZAHL-=0.001;
 	}
 	
 	//Aufrunden wenn noetig
 	if (NACHKOMMA == 0 && ziffer6 >= 5)ziffer5++;
 	if (NACHKOMMA == 1 && ziffer7 >= 5)ziffer6++;
 	if (NACHKOMMA == 2 && ziffer8 >= 5)ziffer7++;
 	
 	//Zahlenueberlauf bei aufrunden verhindern
	while(ziffer7 > 9){
 	if (ziffer7 > 10)ziffer7--; else ziffer7 = 0;
 	ziffer6++;
 	}
 	while(ziffer6 > 9){
 	if (ziffer6 > 10)ziffer7--; else ziffer6 = 0;
 	ziffer5++;
 	}
 	while(ziffer5 > 9){
 	if (ziffer5 > 10)ziffer7--; else ziffer5 = 0;
 	ziffer4++;
 	}
 	while(ziffer4 > 9){
 	if (ziffer4 > 10)ziffer7--; else ziffer4 = 0;
 	ziffer3++;
 	}
 	while(ziffer3 > 9){
 	if (ziffer3 > 10)ziffer7--; else ziffer3 = 0;
 	ziffer2++;
 	}
 	while(ziffer2 > 9){
 	if (ziffer2 > 10)ziffer7--; else ziffer2 = 0;
 	ziffer1++;
 	}
 	//Wenn 0en nicht ausgegeben werden sollen, durch Leerzeichen ersetzen
	if (!PRINTZERO){
		uint8_t zeroc = 0;
		if (ziffer1 == 0){ziffer1 = 0x20; zeroc++;}else{ ziffer1+=0x30;}
		if (ziffer2 == 0 && zeroc == 1){ziffer2 = 0x20; zeroc++;}else{ ziffer2+=0x30;}
		if (ziffer3 == 0 && zeroc == 2){ziffer3 = 0x20; zeroc++;}else{ ziffer3+=0x30;}
		if (ziffer4 == 0 && zeroc == 3){ziffer4 = 0x20; zeroc++;}else{ ziffer4+=0x30;}
		if (ziffer5 == 0 && zeroc == 4){ziffer5 = 0x20; zeroc++;}else{ ziffer5+=0x30;}
	}else{
	 	ziffer1+=0x30;
 		ziffer2+=0x30;
 		ziffer3+=0x30;
		ziffer4+=0x30;
		ziffer5+=0x30;

	}
	//Die Nachkommastellen benoetigen keine Pruefung auf 0stellen
 	ziffer6+=0x30;
 	ziffer7+=0x30;
	ziffer8+=0x30;
	
	//Vorzeichen ausgeben
	if (prefix){
		if (negative){
			uartPutChar('-');
		}else{
			uartPutChar('+');
		}
	}
	//Anzeigen was angezeigt werden soll
	if (VORKOMMA >= 5)uartPutChar(ziffer1);
	if (VORKOMMA >= 4)uartPutChar(ziffer2);
	if (VORKOMMA >= 3)uartPutChar(ziffer3);
	if (VORKOMMA >= 2)uartPutChar(ziffer4);
	if (VORKOMMA >= 1)uartPutChar(ziffer5);
	if (NACHKOMMA >= 1 || VORKOMMA == 0)uartPutChar(',');
	if (NACHKOMMA >= 1 || VORKOMMA == 0)uartPutChar(ziffer6);
	if (NACHKOMMA >= 2)uartPutChar(ziffer7);
	if (NACHKOMMA >= 3)uartPutChar(ziffer8);
}

void uartPrintFloat(float ZAHL, uint8_t VORKOMMA, uint8_t NACHKOMMA, bool prefix){
	uartPrintFloat(ZAHL, VORKOMMA, NACHKOMMA, prefix, 0);
}

void uartPrintFloat(float ZAHL, uint8_t VORKOMMA, uint8_t NACHKOMMA){
	uartPrintFloat(ZAHL, VORKOMMA, NACHKOMMA, 1, 0);
}
//------------------------------------------------------------------------
// setzt Vordergrundfarben und Attribute (Steuercodesequenzen)
//------------------------------------------------------------------------
void uartSetRed(){
	uartPutString("\033[31m");
}

void uartSetGreen(){
	uartPutString("\033[32m");
}

void uartSetYell(){
	uartPutString("\033[33m");
}

void uartSetBlue(){
	uartPutString("\033[34m");
}

void uartSetMag(){
	uartPutString("\033[35m");
}

void uartSetCyan(){
	uartPutString("\033[36m");
}

void uartSetWhite(){
	uartPutString("\033[37m");
}

void uartSetFett(){
	uartPutString("\033[1m");
}

void uartResetFett(){
	uartPutString("\033[22m");
}

//------------------------------------------------------------------------
// Cursor nach oben
//------------------------------------------------------------------------
void uartCursorUp(int count){
		uartPutString("\033[") ;				// Beginn Steuersequenz
		uartPutZahl(count);					// Verschiebewert
		uartPutString("A") ;					// Ende Steuersequenz								
}

//------------------------------------------------------------------------
// Cursor nach unten
//------------------------------------------------------------------------
void uartCursorDown(int count){
		uartPutString("\033[") ;				// Beginn Steuersequenz
		uartPutZahl(count);					// Verschiebewert
		uartPutString("B") ;					// Ende Steuersequenz								
}

//------------------------------------------------------------------------
// Cursor nach rechts
//------------------------------------------------------------------------
void uartCursorRight(int count){
		uartPutString("\033[") ;				// Beginn Steuersequenz
		uartPutZahl(count);					// Verschiebewert
		uartPutString("C") ;					// Ende Steuersequenz								
}

//------------------------------------------------------------------------
// Cursor nach links
//------------------------------------------------------------------------
void uartCursorLeft(int count){
		uartPutString("\033[") ;				// Beginn Steuersequenz
		uartPutZahl(count);					// Verschiebewert
		uartPutString("D") ;					// Ende Steuersequenz								
}

//------------------------------------------------------------------------
// Tonausgabe Putty
//------------------------------------------------------------------------
void uartTon(){
		uartPutString("\033[\a");				// Alarmtonausgabe
}

//--------------------------------------------------------------------
// Reset Terminal
//--------------------------------------------------------------------
void resetTerm(){
	uartPutChar(27);							// Loescht
	uartPutChar(91);							// das gesamte
	uartPutChar(50);							// Terminal-
	uartPutChar(74);							// fenster

	uartPutChar(27);							// Setzt den					
	uartPutChar(91);							// Cursor in
	uartPutChar(59);							// die linke
	uartPutChar(102);						// obere Ecke
}

//Terminal Cursor nach l. oben
void uartHome(){
	uartPutChar(27);							// Setzt den					
	uartPutChar(91);							// Cursor in
	uartPutChar(59);							// die linke
	uartPutChar(102);						// obere Ecke
}

//---------------------------------------------------------------------------
// uartPutZahl Gibt eine 4 Stellige Zahl aus
// PE:uint8_t zahl
//---------------------------------------------------------------------------
void uartPutZahl_1(uint16_t zahl){
 	char ziffer1=0;
 	char ziffer2=0;
 	char ziffer3=0;
 	char ziffer4=0;
 	
 	
 	while(zahl>=1000)
 	{
 	 	ziffer1+=1;
 	 	zahl+=-1000;
 	}
 	while(zahl>=100)
 	{
 	 	ziffer2+=1;
 	 	zahl+=-100;
 	}
 	while(zahl>=10)
 	{
 	 	ziffer3+=1;
 	 	zahl+=-10;
 	}

 	ziffer1+=0x30;
 	ziffer2+=0x30;
 	ziffer3+=0x30;
 	ziffer4=zahl+0x30;
 	uartPutChar(ziffer1);
 	uartPutString(".");
 	uartPutChar(ziffer2);
	uartPutChar(ziffer3);
	//uartPutChar(ziffer4);
}

//Uebergebenen Zeileninhalt loeschen
void lcdClearLine(uint8_t line){
	lcdGoto(line,1);
	lcdWrite("                    ");
	lcdGoto(line,1);
}

//Uebergebenen Zeileninhalt loeschen 4x20 Display
void lcdClearLine_420(uint8_t line){
	lcdGoto_420(line,1);
	lcdWrite("                    ");
	lcdGoto_420(line,1);
}

//FUNKTIONEN WIE BEI ARDUINO


//Pin als Ein bzw. Ausgang setzen
void pinMode(uint8_t PORT, uint8_t PIN, uint8_t MODE){
	
	if (PORT == DDRA){
		if (MODE == 1) DDRA |= (1<<PIN); else DDRA &= ~(1<<PIN);
		
	}else
	if (PORT == DDRB){
		if (MODE == 1) DDRB |= (1<<PIN); else DDRB &= ~(1<<PIN);

	}else
	if (PORT == DDRC){
		if (MODE == 1) DDRC |= (1<<PIN); else DDRC &= ~(1<<PIN);
	
	}else
	if (PORT == DDRD){
		if (MODE == 1) DDRD |= (1<<PIN); else DDRD &= ~(1<<PIN);
	
	}
}

//Pin auf LOW oder HIGH setzen
void digitalWrite(uint8_t PORT, uint8_t PIN, uint8_t STATE){
	
	if (PORT == PORTA){
		if (STATE == 1){ PORTA |= (1<<PIN);} else {PORTA &= ~(1<<PIN);}
		
	}else
	if (PORT == PORTB){
		if (STATE == 1){ PORTB |= (1<<PIN);} else {PORTB &= ~(1<<PIN);}
		
	}else
	if (PORT == PORTC){
		if (STATE == 1){ PORTC |= (1<<PIN);} else {PORTC &= ~(1<<PIN);}
		
	}else
	if (PORT == PORTD){
		if (STATE == 1){ PORTD |= (1<<PIN);} else {PORTD &= ~(1<<PIN);}
	
	}
}

//Pin togglen
void digitalToggle(uint8_t PORT, uint8_t PIN){
  
  if (PORT == PORTA){
    PORTA ^= (1<<PIN);
  }else
  if (PORT == PORTB){
    PORTB ^= (1<<PIN);
  }else
  if (PORT == PORTC){
    PORTC ^= (1<<PIN);
  }else
  if (PORT == PORTD){
    PORTD ^= (1<<PIN);
  }
}

//Status eines Pins einlesen
uint8_t digitalRead(uint8_t PORT, uint8_t PIN){
	if (PORT & (1<<PIN)) return 1;
	return 0;
}


//Auf LCD Text ausgeben an bestimmter stelle
void lcdWriteAt(uint8_t row, uint8_t col,const char* pText){
	lcdGoto(row,col);
	lcdWrite(pText);
}

//Auf LCD Text ausgeben an bestimmter stelle 4x20 Display
void lcdWriteAt_420(uint8_t row, uint8_t col,const char* pText){
	lcdGoto_420(row,col);
	lcdWrite(pText);
}

//Auf LCD Text ausgeben an bestimmter stelle
void lcdWriteAt(uint8_t row, uint8_t col,char pText){
	lcdGoto(row,col);
	lcdWrite(pText);
}

//Auf LCD Text ausgeben an bestimmter stelle 4x20 Display
void lcdWriteAt_420(uint8_t row, uint8_t col,char pText){
	lcdGoto_420(row,col);
	lcdWrite(pText);
}

//Cursor auf dem Display deaktivieren
void lcdDisableCursor(){
	lcdCmd(0x0C);
}

//Eingegebenen X Wert in einem Bereich von in_min bis in_max auf einen Wert zwischen out_min bis out_max bringen
float map(float x, float in_min, float in_max, float out_min, float out_max){
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

//Funktion, welche es ermoeglicht, Interrupts einfach und uebersichtlich zu initialisieren
void addExtInt(uint8_t PIN, uint8_t STATE){
  
  //Ueberpruefen ob uebergebene Werte gueltig sind
  if ((STATE >=0 && STATE <= 3) && (PIN == INT0 || PIN == INT1 || PIN == INT2)) {
    //Interrupt Pin ins Register schreiben
    GICR |= (1<<PIN);
    //Zu ueberwachende aenderung auswaehlen
    switch(STATE){
      
      //Wenn LOW -> Registerwerte setzen
      case LOW:{
        if (PIN == INT0) {
          MCUCR &= ~(1<<ISC00);
          MCUCR &= ~(1<<ISC01);
        }else
        if (PIN == INT1){
          MCUCR &= ~(1<<ISC10);
          MCUCR &= ~(1<<ISC11);
        }
        break;
      }
      //Wenn steigende Flanke -> Registerwerte setzen
      case RISING:{
        if (PIN == INT0) {
          MCUCR |= (1<<ISC00);
          MCUCR |= (1<<ISC01);
        }else
        if (PIN == INT1){
          MCUCR |= (1<<ISC10);
          MCUCR |= (1<<ISC11);
        }else{
            MCUCSR |= (1<<ISC2);
        }
        break;
      }
      //Wenn fallende Flanke -> Registerwerte setzen
      case FALLING:{
        if (PIN == INT0) {
          MCUCR &= ~(1<<ISC00);
          MCUCR |= (1<<ISC01);
        }else
        if (PIN == INT1){
          MCUCR &= ~(1<<ISC10);
          MCUCR |= (1<<ISC11);
        }else{
			MCUCSR &= ~(1<<ISC2);
		}
        break;
      }
      //Wenn zustandsaenderung -> Registerwerte setzen
      case CHANGE:{
        if (PIN == INT0) {
          MCUCR |= (1<<ISC00);
          MCUCR &= ~(1<<ISC01);
        }else
        if (PIN == INT1){
          MCUCR |= (1<<ISC10);
          MCUCR &= ~(1<<ISC11);
        }
        break;
      }
    }
  }
}

void uartInit(){
  sbi(UCSRB,3);	//TX Aktivieren
  sbi(UCSRB,4);	//RX Aktivieren
  UBRRL  = (uint8_t)(F_CPU / (BAUD * 16L))-1;
  UBRRH  = (uint8_t)((F_CPU / (BAUD * 16L))-1) >>8;
  UCSRC = (1<<URSEL)|(1<<UCSZ1)|(1<<UCSZ0);
  sbi(UCSRB,7);	//RXC Interrupt aktivieren
}

void uartInit(uint8_t ubrrl, uint8_t ubrrh){
	uartInit();
	UBRRL = ubrrl;
	UBRRH = ubrrh;
	
}

void uartInit(bool interrupt){
	uartInit();
	cbi(UCSRB,7);	//RXC Interrupt aktivieren
}

void uartNextLine(){
  uartPutString("\n\r");
}

void uartResetCursor(){
  uartPutString("\033[H");
}

void adcInit(uint8_t VORTEILER){
  //ADC Initialisieren
  ADCSRA = 0x00;
  
  switch(VORTEILER){
    
    case 2:{
      ADCSRA |= 0;
      break;
    }
    
    case 4:{
      ADCSRA |= 2;
      break;
    }
    
    case 8:{
      ADCSRA |= 3;
      break;
    }
    
    case 16:{
      ADCSRA |= 4;
      break;
    }
    
    case 32:{
      ADCSRA |= 5;
      break;
    }
    
    case 64:{
      ADCSRA |= 6;
      break;
    }
    
    case 128:{
      ADCSRA |= 7;
      break;
    }
    
    default:{
      ADCSRA |= 7;
      break;
    }
  }
  
    ADCSRA |= 0xE0;
}


//Normales Auslesen des ADC
uint16_t adcRead(uint8_t PIN){
	if (PIN >= 0 && PIN <= 7) {
		ADMUX = 0x00;
		ADMUX |= PIN;
		ADMUX |= 0xC0;
	}
	uint16_t VALUE1 = ADC;
	unsigned char VALUE2 = ADCH;
	return VALUE1;
  
}

uint16_t adcRead(uint8_t PIN, uint16_t start, uint16_t stop,uint8_t bit){
	//Wenn adc normal ausgelesen werden soll
	if (start == 0 && stop == 0){
		uint16_t VALUE1 = adcRead(PIN);
		return VALUE1;
	}else{
		uint16_t VALUE1 = adcRead(PIN);
		if (bit == 8){
			return map(VALUE1,0,255,start,stop);
		} else {
			return map(VALUE1,0,1023,start,stop);
		}

	}
}

float adcRead(uint8_t PIN, long in_min, long in_max, long out_min, long out_max){
	return map(adcRead(PIN),in_min, in_max, out_min, out_max);
}

void uartPutStringln(const char *buffer){
 	for (int i=0; buffer[i] !=0;i++)
 	 	uartPutChar (buffer[i]);
	uartNextLine();
}
