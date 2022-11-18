// LCD1602 to Arduino Uno connection example

#include <LiquidCrystal.h>
#include <avr/io.h>
#include <avr/interrupt.h>

LiquidCrystal lcd(12, 11, 10, 9, 8, 7);

const int buttonPin = 2;
const int builtinLedPin = 13;
const int ledPin = 5;
int i = 1000;
unsigned int reload = 2000; 
unsigned int reload2 = 156;
volatile int count;
volatile int count2;
String result;
int test;
bool flagRetour = false;
int wait = 0;

ISR(TIMER1_COMPA_vect)
{
count++;
flash();
}

ISR(TIMER2_COMPA_vect)
{
  count2++;
  OCR2A = reload2;
}

void setup() {
  Serial.begin(115200);
  pinMode(builtinLedPin, OUTPUT);
  digitalWrite(builtinLedPin, LOW);
  lcd.begin(16, 2);
  pinMode(buttonPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(buttonPin), onEvent, FALLING);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  lcd.clear();

  cli();
  TCCR1A = 0;
  TCCR1B = 0; 
  OCR1A = reload;
  TCCR1B = (1<<WGM12) | (1<<CS12); 
  TIMSK1 = (1<<OCIE1A); 

  TCCR0B = 0;
  OCR2A = reload2;
  TCCR2A = 1<<WGM21;
  TCCR2B = (1<<CS22) | (1<<CS21) | (1<<CS20);
  TIMSK2 = (1<<OCIE2A);
  sei(); 
  // you can now interact with the LCD, e.g.:
  lcd.print("DEBUT DU TEST !");
}

void loop() {
  if(count2 == 50)
  {
    static boolean output2 = HIGH;
    digitalWrite(builtinLedPin, output2);
    output2 = !output2;
    count2 = 0;
    //TEST
    OCR1A -= 10;
    //FINTEST
  }
}

void onEvent()
{
  lcd.clear();
  test = 1/(OCR1A * 0.000016);
  result = String(test);
  Serial.print("OCR1A :" + String(OCR1A) + " Freq : ");
  Serial.print(String(1/(OCR1A*0.000016)) + "Hz");
  Serial.println();
  lcd.print("result: " + result + "Hz");
  delay(1000);
}

void flash()
{
  static boolean output = HIGH;
  digitalWrite(ledPin, output);
  output =! output;

  // if(OCR1A != 1 && flagRetour == false)
  // {
  //   OCR1A -= 1;
  // }
  // else
  // {
  //   if(flagRetour == false)
  //   {
  //     lcd.clear();
  //     lcd.print("TEST INVERSE :");
  //     digitalWrite(ledPin, HIGH);
  //     while(wait < 500)
  //     {
  //       wait += 1;
  //       Serial.print(wait);
  //       Serial.println();
  //     }
  //     wait = 0;
  //     while(wait < 500)
  //     {
  //       wait += 1;
  //       Serial.print(wait);
  //       Serial.println();
  //     }
  //     wait = 0;
  //     lcd.clear();
  //     lcd.print("3");
  //     while(wait < 500)
  //     {
  //       wait += 1;
  //       Serial.print(wait);
  //       Serial.println();
  //     }
  //     wait = 0;
  //     lcd.clear();
  //     lcd.print("2");
  //     while(wait < 500)
  //     {
  //       wait += 1;
  //       Serial.print(wait);
  //       Serial.println();
  //     }
  //     wait = 0;
  //     lcd.clear();
  //     lcd.print("1");
  //     while(wait < 500)
  //     {
  //       wait += 1;
  //       Serial.print(wait);
  //       Serial.println();
  //     }
  //     lcd.clear();
  //     lcd.print("GO !");
  //   }
  //   flagRetour = true;

  //   OCR1A += 1;
  //}
}