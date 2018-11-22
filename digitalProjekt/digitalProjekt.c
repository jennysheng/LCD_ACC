/*
 * Lab4_right_.c *
 * Created: 2013-11-13 11:27:26
 * Author: Jenny
 */ 
#include <avr/io.h>
#include <util/delay.h> //För att _delay_ms() ska fungera
#include <avr/interrupt.h>
#include <stdlib.h>
#include <string.h>
#define F_CPU 16000000UL
#define BaudRate 9600
#define MYUBRR (F_CPU / 16 / BaudRate ) - 1


/************************************************************************
*Namn: putch_lc
*
*Syfte: lägga ut ett tecken på displayen *
*
*Indata: tecknet som skall skickas till displayen *
*
*Utdata: void
*
*Anropar:_delay_ms()
************************************************************************/
void putch_lcd(unsigned char data)
{
	PORTB = (1<<PC5)|(1<<PC4)|((data & 0xF0) >> 4);//E=1,RS=1 samt skickar de 4 höga-bitarna av tecknet till PORTC 0-3
	PORTB &= ~(1<<PC5); // E=0 nödvändig för att dislpayen ska läsa in datan
	_delay_us(1);
	PORTB = (1<<PC5)|(1<<PC4)|(data & 0x0F); //E=1,RS=1 samt skickar de 4 låga-bitarna av tecknet till PORTC 0-3
	PORTB &= ~(1<<PC5); // E=0
	_delay_ms(5); //väntar på displayen
}

/************************************************************************
*Namn: putcmd_lcd
*
*Syfte: skicka ett komando till displayen *
*
*Indata: kommandet som ska skickas
*
*Utdata: void
*
*Anropar:_delay_us(1)
************************************************************************/
void putcmd_lcd(unsigned char data)
{
	PORTB = (1<<PC5)|((data & 0xF0) >> 4);//E=1 samt skickar de 4 höga-bitarna av komandot till PORTC 0-3
	PORTB &= ~(1<<PC5); // E=0 nödvändig för att displayen ska läsa in datan
	_delay_us(1);
	// LCD Lower 4 bits data (DB3,DB2,DB1,DB0)
	PORTB = (1<<PC5)|(data & 0x0F); //E=1 samt skickar de 4 låga-bitarna av komandot till PORTC 0-3
	PORTB &= ~(1<<PC5);
	_delay_ms(5); // vänta på displayen
}

/************************************************************************
*Namn: init_lcd
*
*Syfte: initiera lcd-displayen(dmc16106c) *
*
*Indata: void
*
*Utdata: void
*
*Anropar:putcmd_lcd()
************************************************************************/
void init_lcd()
{
	_delay_ms(30); // väntar så att displayen hinner vakna
	putcmd_lcd(0x28); // sätter displayen i 4 bit mode och till två rader
	putcmd_lcd(0x0C); // display på, cursor av, blink av
	putcmd_lcd(0x01); // rensar displayen och sätter cursorn till position 0( längst till vänster på rad 1)
	putcmd_lcd(0x06); // Entry Mode Set: Increment, No shift
}
/*************************************************************************/
//interrupt

ISR(INT0_vect){		
	
	DDRD = 1<<PIND7;		//tända en lysioide på pin7
	PORTD = 1<<PIND7;	
	putcmd_lcd(0x01);       //resa skämen
	_delay_ms(10000);
	PORTD=0b00000000;        //nullställa portd
	}
	
	
	

/***********************************************************************
*Namn: main
*
*Syfte: behöver finnas
*
*Indata: void
*
*Utdata: en int
*
*Anropar:init_lcd(), putcmd_lcd(), putch_lcd(), _delay_ms() *
************************************************************************/
char m[4];
char n[4];
char s[4];
int count = 0;
int counter = 0;
int cou =0;
uint16_t X, Y, Z; //unsigned 16-bit integer

void taemotX(){
	itoa(X, s,10); //interger till asc code
	s[counter] = X; //tar emot X satt pa s array;
	counter++;
	putcmd_lcd(0x81);
	putch_lcd(s[0]); //skriver ut s;
	_delay_ms(1000);
	putch_lcd(s[1]);
	_delay_ms(1000);
	putch_lcd(s[2]);
	_delay_ms(1000);
	
	
}
void taemotY(){
	itoa(Y, m, 10);
	m[count]  =  Y;
	count++;
	putcmd_lcd(0x86);
	putch_lcd(m[0]);
	_delay_ms(1000);
	putcmd_lcd(0x87);
	putch_lcd(m[1]);
	_delay_ms(1000);
	putcmd_lcd(0x88);
	putch_lcd(m[2]);
	_delay_ms(1000);
}

void taemotZ(){
	itoa(Z, n, 10);
	n[cou]  =  Z;
	cou++;
	putcmd_lcd(0x8B);
	putch_lcd(n[0]);
	_delay_ms(1000);
	
	if(n[1]>0){
		putcmd_lcd(0x8C);
		putch_lcd(n[1]);
		_delay_ms(1000);}else{
		putcmd_lcd(0x8C);
		putch_lcd(0x11);
		putch_lcd(0x11);
		putch_lcd(0x11);
		putch_lcd(0x11);
		
	}
	if(n[2]>0){
		putcmd_lcd(0x8D);
		putch_lcd(n[2]);
		_delay_ms(1000);	}else{
		putcmd_lcd(0x8D);
		putch_lcd(0x11);
		putch_lcd(0x11);
		
	}
	if(n[3]>0){
		putcmd_lcd(0x8E);
	putch_lcd(n[3]);}
	else{
		putcmd_lcd(0x8E);
		putch_lcd(0x11);
	}
	
}

void ADC_omvandlingX(){
	ADMUX = (ADMUX & 0xf0 | 0x00);//satt den vanster 4 bit som utgång och adc for x
	ADCSRA |= 0x40;//start ADC
	while (ADCSRA & (1<<ADSC)){}// vanta tills adc  klar
	X=ADC;// hamta varde
}

void ADC_omvandlingY(){
	ADMUX = (ADMUX & 0xf0 | 0x01);//satt den vanster 4 bit som utgång och adc for x
	ADCSRA |= 0x40;
	while (ADCSRA & (1<<ADSC)){}
	Y=ADC;
}

void ADC_omvandlingZ(){
	ADMUX = (ADMUX & 0xf0 | 0x02);
	ADCSRA |= 0x40;
	while (ADCSRA & (1<<ADSC)){}//start conversion bit 6
	Z=ADC;
}


void initADC(){
	ADMUX = 0b01000000;//0 1 AVCC with external capacitor at AREF pin
	ADCSRA = 0;
	ADCSRA |= (1<<ADEN);//Writing this bit to one enables the ADC.
	ADCSRA |= (1<<ADPS0);//Timer/Counter1 Capture Event
	ADCSRA |= (1<<ADPS1);
	ADCSRA |= (1<<ADPS2);
	
}

int main(void)
{
	DDRB = 0xFF; // PORTB sätts till utgång
	PORTB = 0xFF; //nollställer PORTB	
	init_lcd(); //initierar displayen
	initADC();	
	//---------------------------------------------------------------interruptSetup();
	EIMSK = 1<<INT0;					// Enable INT0
	EICRA = 1<<ISC01 | 0<<ISC00;	// Trigger INT0 on following edge
	
    DDRD = 1<<PD2;		// Set PD2 as input (Using for interupt INT0)
    PORTD = 1<<PD2;		// Enable PD2 pull-up resistor	
	sei();		//Enable Global Interrupt
	while(1){
	
		ADC_omvandlingX();	
		ADC_omvandlingY();	
		ADC_omvandlingZ();
		
		
	putcmd_lcd(0x80); //ställer kursern på rad ett plats ett
	putch_lcd(0x58);//Skriver ut text received från rx på displayen
	putcmd_lcd(0x85); //ställer kursern på rad ett plats ett
	putch_lcd(0x59);//Skriver ut text received från rx på displayen
	putcmd_lcd(0x8A); //ställer kursern på rad ett plats ett
	putch_lcd(0x5A);//Skriver ut text received från rx på displayen 
	taemotX();
	taemotY();	
	taemotZ();	
	}
		
	
}


		
		
		
	
	


	
	
	