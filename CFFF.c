/* Critical flicker fusion frequency circuit controller

Flashes an LED at a frequency controlled by a rotary encoder, displays
the frequency on a LCD, controls the brightness of the LED with a pot,
controls the color of the LED with a button.

Uses three transistors to multiply a low-frequency PWM pin (the one that does 
the visible low-frequency blinking) controlled with the TimerOne library with
three regular high-frequency PWM pins controlled by the usual digitalOutput.
*/

#include <TimerOne.h>

// These two correspond to interrupts, so only work
// with pins 2 & 3 on the Uno.
// Button that controls the color of the LED.
const int buttonPin = 3;

// Quad encoder pin A
const int encoder0PinA = 2;

// For the other part of the quad encoder.
const int encoder0PinB = 12;  

// The slow modulation pin.
const int slowPin = 9;

// The fast modulation pins.
const int redPin = 11;
const int greenPin = 6;
const int bluePin = 5;

// The potentiometer has to be on an analog pin to be read.
const int potInput = A5;

// Move .5 Hz per rotation.
const int rotaryMultiplier = 2; 

volatile unsigned int encoder0Pos = 2; // Start at encoder0Pos / rotaryMultiplier (= 1) Hz
volatile unsigned int oldLedLum = 128; // Start at 50% luminance.
volatile unsigned int colorState = 0; // Start in the red state.

// Include the library code for the LCD.
#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins.
LiquidCrystal lcd(7, 4, A0, A1, A2, A3);

void setup() { 
  // Display a friendly message on the LCD.
  lcd.begin(16, 2);
  lcd.print("Fusion freq circ");
  lcd.setCursor(0, 1);
  lcd.print("Ready");

  // Prepare the quad encoder.
  pinMode(encoder0PinA, INPUT); 
  digitalWrite(encoder0PinA, HIGH);
  pinMode(encoder0PinB, INPUT); 
  digitalWrite(encoder0PinB, HIGH);
  // Note: we use an interrupt to catch the quad encoder event.
  attachInterrupt(digitalPinToInterrupt(encoder0PinA), quadEncoderCallback, CHANGE);
  
  // Initialize the timer on the special low frequency modulator pin.
  Timer1.initialize(rotaryMultiplier * 1000000 / encoder0Pos);
  
  // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT);
  
  // Attach an interrupt to the push button callback
  attachInterrupt(digitalPinToInterrupt(buttonPin), pushButtonCallback, CHANGE);

  // Initialize the pins for the RGB LED.
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
} 

const int nstates = 4;
void loop(){
  // Always set the slow pin so that it has a 50% duty cycle.
  // 512 / 1024 == 50% duty cycle.
  Timer1.pwm(slowPin, 512);
  
  // Read the luminance of the LED from the potentiometer.
  int ledLum = map(analogRead(potInput), 0, 1023, 0, 255);
  if(ledLum != oldLedLum) {
    oldLedLum = ledLum;
  }
  
  // Set it so that there are 4 states, with the first being red, the second 
  // green, the third blue, and the fourth off.
  if(colorState % nstates == 0) {
      analogWrite(redPin, ledLum);
      analogWrite(greenPin, 0);
      analogWrite(bluePin, 0);
  } else if(colorState % nstates == 1) {
      analogWrite(redPin, 0);
      analogWrite(greenPin, ledLum);
      analogWrite(bluePin, 0);
  } else if(colorState % nstates == 2) {
      analogWrite(redPin, 0);
      analogWrite(greenPin, 0);
      analogWrite(bluePin, ledLum);
  } else {
      analogWrite(redPin, 0);
      analogWrite(greenPin, 0);
      analogWrite(bluePin, 0);
  }
}

void quadEncoderCallback() {
  /* If pinA and pinB are both high or both low, it is spinning
   * forward. If they're different, it's going backward.
   *
   * For more information on speeding up this process, see
   * [Reference/PortManipulation], specifically the PIND register.
   */
  if (digitalRead(encoder0PinA) == digitalRead(encoder0PinB)) {
    encoder0Pos++;
  } else {
    // Don't let it roll over.
    if(encoder0Pos != 0) {
      encoder0Pos--;
    }
  }
  
  // Set the frequency of the slow pin.
  Timer1.initialize( rotaryMultiplier * 1000000 / encoder0Pos); // in microseconds

  // And update the LCD display so it shows some nice feedback.
  lcd.setCursor(0, 0);
  lcd.clear();
  
  // Ideally you'd want a sprintf here to display a float here, but in the 
  // absence of this, print the integer part first, then multiply by 10, 
  //mod 10 to get the first decimal, and print that.
  float freq = float(encoder0Pos) / float(rotaryMultiplier);
  lcd.print(int(freq));
  lcd.print('.');
  lcd.print((int(freq * 10) % 10));
  lcd.print(" Hz");
}

void pushButtonCallback() {
  // Cycle through the color states.
  int buttonState = digitalRead(buttonPin);
  if(buttonState == 0) {
    colorState++;
  }
}