
//!!!!ONLY FOR TESTING PURPOSES!

#define FILENAME "DCF77gen3"
// Generator for 77,5 kHz (DCF-77) by Jonas woerner (c)
#include <Time.h>
byte dcfBits[60] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0 };
//int Minuten = 15;
//int Stunden = 12;
const int anzahlMinutenBits = 7;
const int anzahlStundenBits = 6;
const int offsetMinutenBits = 21;
const int offsetStundenBits = 29;
byte MinutenBits[anzahlMinutenBits] = { 0 };
byte StundenBits[anzahlStundenBits] = { 0 };
int parity = 0;
// lowered signal. always on:
const byte pin1  =  11;    // = OC2A. goes to antenna
const byte pin2  =  12;    // set to LOW. goes via  22 ohms to antenna
const byte led1  =  10;
const byte led2  =   8;
const int stdled = 13;
//                                    GND goes via 180 ohms to antenna
/*      +----------+
GND ----| 180 ohms |----+         <-------- always connected to ground
        +----------+    |
                        |
        +----------+    |
pin2 ---|  22 ohms |----+-----+   <-------- high power when set to OUTPUT
        +----------+    |     |
                        |     /
                      -----   \
      capacitor --->          /   <-------- antenna
                      -----   \
                        |     /
                        |     |
pin1 -------------------+-----+       
*/
void setup()
{
  DCF_BITS(15,13);  //set time to 15:13
   pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(pin1, OUTPUT);
  pinMode(stdled,OUTPUT);
  pinMode(pin2, OUTPUT);
  digitalWrite(pin2, LOW);
  digitalWrite(stdled,HIGH);
//TCCR2B = B00000001;            
  // Bits 2 1 0 = 0 0 1 : no prescaling, divide by 1 (other prescalers: 8, 32, 64, 128, 256, 1024)
  //TCCR2A = B01000010;  
  // | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
  // | 0 | 1 | 0 | 0 | - | - | - | - | : Toggle OC2A on Compare Match
  // | - | - | - | - | 0 | 0 | 1 | 0 | : Clear Timer on Compare Match  
  //OCR2A  = 102;    // frequency = 16,000,000 / 2 / (OCR2A + 1) = 77.604 kHz
  // each time when Timer2 gets 102 the selected output gets inverted
     setupFrequencyGen();
  //206
  setFrequency(206);        //-Make 77KHz
    //analogWrite(11,255);
}

void loop()
{
  
  for (int i = 0;i<58;++i)
  {
    modulate(dcfBits[i]);
    delay(1000);
  }
  setLevel(1);
  digitalWrite(stdled,HIGH);
  delay(1000);
}


void DCF_BITS(int Minuten, int Stunden)
{
  
  //MINUTE\\

		if (Minuten > 39)
	{
		MinutenBits[6] = 1;
		Minuten -= 40;
	}
		if (Minuten > 19)
	{
		MinutenBits[5] = 1;
		Minuten -= 20;
	}
	
		if (Minuten > 9)
	{
		MinutenBits[4] = 1;
		Minuten -= 10;
	}

	for (int i = 0; i < 4; ++i)
	{
		//MinutenBits[i] = (Minuten & (1 << i)) ? true : false;

		if ((Minuten & (1 << i)) > 0)
		{
			MinutenBits[i] = true;
		}
		else
		{
			MinutenBits[i] = false;
		}
	}
	for (int i = 0; i < anzahlMinutenBits; ++i)
			{
				dcfBits[offsetMinutenBits + i] = MinutenBits[i];
			}
	//Stunde\\

		if (Stunden > 19)
	{
		StundenBits[5] = 1;
		Stunden -= 20;
	}
		 if (Stunden > 9)
	{
		StundenBits[4] = 1;
		Stunden -= 10;
	}
	 

	

	 for (int i = 0; i < 4; ++i)
	 {
		 //MinutenBits[i] = (Minuten & (1 << i)) ? true : false;

		 if ((Stunden & (1 << i)) > 0)
		 {
			 StundenBits[i] = true;
		 }
		 else
		 {
			 StundenBits[i] = false;
		 }
	 }

	 for (int i = 0; i < anzahlStundenBits; ++i)
	 {
		 dcfBits[offsetStundenBits + i] = StundenBits[i];
	 }

	 /*for (int n = 0; n < 6; ++n)
	 {

	 }*/
	 ////////////////////////////
	 {
		 parity += dcfBits[21];
		 parity += dcfBits[22];
		 parity += dcfBits[23];
		 parity += dcfBits[24];
		 parity += dcfBits[25];
		 parity += dcfBits[26];
		 parity += dcfBits[27];
		 if (parity % 2 == 0)
		 {
			 dcfBits[28] = 0;
		 }
		 else
		 {
			 dcfBits[28] = 1;
		 }
		 parity = 0;
	 }
	 ////////////////////////////
	 {
		 parity += dcfBits[29];
		 parity += dcfBits[30];
		 parity += dcfBits[31];
		 parity += dcfBits[32];
		 parity += dcfBits[33];
		 parity += dcfBits[34];
		 if (parity % 2 == 0)
		 {
			 dcfBits[35] = 0;
		 }
		 else
		 {
			 dcfBits[35] = 1;
		 }
		 parity = 0;
	 }
	 /////////////////////////////
	 {
		 parity += dcfBits[36];
		 parity += dcfBits[37];
		 parity += dcfBits[38];
		 parity += dcfBits[39];
		 parity += dcfBits[40];
		 parity += dcfBits[41];
		 parity += dcfBits[42];
		 parity += dcfBits[43];
		 parity += dcfBits[44];
		 parity += dcfBits[45];
		 parity += dcfBits[46];
		 parity += dcfBits[47];
		 parity += dcfBits[48];
		 parity += dcfBits[49];
		 parity += dcfBits[50];
		 parity += dcfBits[51];
		 parity += dcfBits[52];
		 parity += dcfBits[53];
		 parity += dcfBits[54];
		 parity += dcfBits[55];
		 parity += dcfBits[56];
		 parity += dcfBits[57];
		 if (parity % 2 == 0)
		 {
			 dcfBits[58] = 0;
		 }
		 else
		 {
			 dcfBits[58] = 1;
		 }
		 parity = 0;
	 }
}

void setLevel(byte b) {
  switch (b) {
    case 0: pinMode(pin2, INPUT); break;  // open   = low sinal
    case 1: pinMode(pin2, OUTPUT); break; // closed = high signal
  }
}
void modulate(byte b)
{
  
   if (b == 0)
   {
     digitalWrite(stdled,LOW);
     pinMode(pin2, INPUT);  // open   = low sinal
     delay(100);
     pinMode(pin2, OUTPUT);  // closed = high signal
     digitalWrite(stdled,HIGH);
   }
   else
   {
     digitalWrite(stdled,LOW);
      pinMode(pin2, INPUT);  // open   = low sinal
     delay(200);
     pinMode(pin2, OUTPUT);  // closed = high signal
     digitalWrite(stdled,HIGH);
   }
  
}

void setFrequency(int d)
{
  //Frequency = 16000000/d

  if(d<0)
    d=-d;
  if(d<2)
    d=2;
  TCCR1B&=0xfe;             //-Stop generator
  TCNT1=0;                  //-Clear timer
  ICR1=d;                   // |
  OCR1A=(d/2);              //-+
  TCCR1B|=0x01;             //-Restart generator
}

void setupFrequencyGen()
{
  TCCR1A=0b10000010;        //-Set up frequency generator
  TCCR1B=0b00011001;        //-+
  setFrequency(16);         //-Start with 1MHz
  pinMode(11,OUTPUT);        //-Signal generator pin
}

