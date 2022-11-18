//Critical Fusion Flicker Frequency with an arduino UNO board and a regular LED from the starter kit.
//Displaying of the result on an LCD display
#include <LiquidCrystal.h>
#include <avr/io.h>
#include <avr/interrupt.h>

//Pins of the lcd display
LiquidCrystal lcd(12, 11, 10, 9, 8, 7);

//Pin of the button to press to set the CFF
const int buttonPin = 2;
//Use of the builtin led for test
const int builtinLedPin = 13;

//Pin of the LED that is used for the test
const int ledPin = 5;

//Value to compare the first timer
unsigned int reload = 2000; 
//Value to compare the second timer
unsigned int reload2 = 156;

//Counter for the first timer
volatile int count;
//Counter for the second timer
volatile int count2;

//String for displaying result
String result = "";
//For computing Hz
int resultInt;
//To know if the test is forward or backward.
bool flagRetour = false;
//For setting a delay inside an interruption
int wait = 0;

//Interruption on timer 1
ISR(TIMER1_COMPA_vect)
{
count++;
flash();
}

//Interruption on timer 2
ISR(TIMER2_COMPA_vect)
{
  count2++;
  OCR2A = reload2;
}

void setup() {
  Serial.begin(115200);
  //Set the builtin led to output
  pinMode(builtinLedPin, OUTPUT);
  //Turn off the builtin led
  digitalWrite(builtinLedPin, LOW);
  //Initialize the LCD display
  lcd.begin(16, 2);
  //Set the button as an Input Pullup
  pinMode(buttonPin, INPUT_PULLUP);
  //Interrupt on rising event on the button
  attachInterrupt(digitalPinToInterrupt(buttonPin), onEvent, RISING);
  //Set the led as an output
  pinMode(ledPin, OUTPUT);
  //Turn off the led
  digitalWrite(ledPin, LOW);
  //Clear the LCD in case there is something written on it
  lcd.clear();

  //Stopping all interruptions
  cli();
  //Setting of the register for the Timer 1
  TCCR1A = 0;
  TCCR1B = 0; 
  OCR1A = reload;
  TCCR1B = (1<<WGM12) | (1<<CS12); 
  TIMSK1 = (1<<OCIE1A); 

  //Setting of the register for the Timer 2
  TCCR0B = 0;
  OCR2A = reload2;
  TCCR2A = 1<<WGM21;
  TCCR2B = (1<<CS22) | (1<<CS21) | (1<<CS20);
  TIMSK2 = (1<<OCIE2A);
  sei(); 
  //Resume all interruptions

  //Print to show everything has been initialized correctly
  lcd.print("DEBUT DU TEST !");
}

void loop() {
  //Every 50 interruptions of timer 2 (500ms) -->
  if(count2 == 50)
  {
    //Reset of the counter
    count2 = 0;
    //Si < 1000 Hz
    if(OCR1A > 200 && flagRetour == false)
    {
      //+ 1/4 Hz
      OCR1A -= 18;
    }
    else
    {
      //flagRetour allows to identify the end of the forward test
      if(flagRetour == false)
      {
        lcd.clear();
        lcd.print("TEST INVERSE :");
        digitalWrite(ledPin, HIGH);
        while(wait < 1500)
        {
          wait += 1;
          Serial.print(wait);
          //Serial.println();
        }
        wait = 0;
        while(wait < 1500)
        {
          wait += 1;
          Serial.print(wait);
          //Serial.println();
        }
        wait = 0;
        lcd.clear();
        lcd.print("3");
        while(wait < 1500)
        {
          wait += 1;
          Serial.print(wait);
         // Serial.println();
        }
        wait = 0;
        lcd.clear();
        lcd.print("2");
        while(wait < 1500)
        {
          wait += 1;
          Serial.print(wait);
         // Serial.println();
        }
        wait = 0;
        lcd.clear();
        lcd.print("1");
        while(wait < 1500)
        {
          wait += 1;
          Serial.print(wait);
          //Serial.println();
        }
        lcd.clear();
        lcd.print("GO !");
        Serial.println();
      }
      flagRetour = true;
      //-1/4 Hz
      OCR1A += 18;
      //Reset counter
      count2 = 0;
    }
  }
}

//Button pushed
void onEvent()
{
  //Clearing the display
  lcd.clear();
  //Transform the Hz result to an int
  resultInt = 1/(OCR1A * 0.000016);
  //Then to a string
  result = String(resultInt);
  //Send the result to the serial monitor
  Serial.print("OCR1A :" + String(OCR1A) + " Freq : ");
  Serial.print(String(1/(OCR1A*0.000016)) + "Hz");
  Serial.println();
  //Display the result on the screen
  lcd.print("result: " + result + "Hz");
}

//Changing the output state of the LED
void flash()
{
  static boolean output = HIGH;
  digitalWrite(ledPin, output);
  output =! output;
}