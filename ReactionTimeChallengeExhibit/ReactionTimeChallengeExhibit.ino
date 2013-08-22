/**************************************************************
*      Reaction Time Challenge Exhibit
*          by Bill Porter
*              www.billporter.info
*
*    For more info, visit:
*          http://www.billporter.info/?p=1375
*
*
*
***************************************************************/


/*    Operation:

The exhibit is a player vs player game on who has the fastest reaction times. 
It has two large player buttons and 2x 1m long addressable RGB led strips to show the reaction times.
A start button will play out a random count, when the strips turn green, the players hit the button. 
How far up the strips light up shows your reaction time.

Start button is read during all the delays in the Adafruit animation code. 

The start and player buttons are backlit. Figure the start button will blink during idle to attrack attention. 
When the game starts, both player buttons will light up, then the player who wins button will flash. 

We will use hardware SPI to talk to the strips.

If one strip is 32 LEDS for 1 second, each LED represents 31.25 mS or about 31 ms. 
*/

/*  Wiring

Digital Pins
0  UART
1  UART
2  Player 1 button
3  Player 2 button
4  Start Button
5  Player 1 button LED
6  Player 2 button LED
7
8  Speaker
9  Start Button LED
10
11  LED Strip Data Out
12
13  LED Strip Clock

*/

#define NOTE_A3  220
#define NOTE_A4  440
#define NOTE_A5  880

#define PLAYER1BUTTON 2
#define PLAYER2BUTTON 3
#define STARTBUTTON 4
#define PLAYER1LED 5
#define PLAYER2LED 6
#define SPEAKER 8
#define STARTLED 9

#define NUMPIXELS 60
#define LASTPIXEL 59

//#include <avr/pgmspace.h>
#include <SPI.h>
#include "LPD8806.h"
//#include "TimerOne.h"

//adafruit's control
const int numPixels = NUMPIXELS;
//start LED strip Library
LPD8806 strip = LPD8806(NUMPIXELS);

long lastBlink;



void setup(){
  
  //setup pins
  pinMode(PLAYER1BUTTON, INPUT);
  pinMode(PLAYER2BUTTON, INPUT);
  pinMode(STARTBUTTON, INPUT);
  pinMode(PLAYER1LED, OUTPUT);
  pinMode(PLAYER2LED, OUTPUT);
  pinMode(STARTLED, OUTPUT);
  
  //enable pull-ups
  digitalWrite(PLAYER1BUTTON, HIGH);
  digitalWrite(PLAYER2BUTTON, HIGH);
  digitalWrite(STARTBUTTON, HIGH);
  
  // Start up the LED strip
  strip.begin();
  
  // Update the strip, to start they are all 'off'
  strip.show();
  
  randomSeed(analogRead(0));

  
  Serial.begin(119200);
}


void loop(){
  
  // Send a simple pixel chase in...
  colorChase(strip.Color(127,127,127), 20); // white
  colorChase(strip.Color(127,0,0), 20);     // red
  colorChase(strip.Color(127,127,0), 20);   // yellow
  colorChase(strip.Color(0,127,0), 20);     // green
  colorChase(strip.Color(0,127,127), 20);   // cyan
  colorChase(strip.Color(0,0,127), 20);     // blue
  colorChase(strip.Color(127,0,127), 20);   // magenta

  // Fill the entire strip with...
  colorWipe(strip.Color(127,127,127), 20);  // White
  colorWipe(strip.Color(127,0,0), 20);      // red
  colorWipe(strip.Color(127,127,0), 20);    // yellow
  colorWipe(strip.Color(0, 127,0), 20);     // green
  colorWipe(strip.Color(0,127,127), 20);    // cyan
  colorWipe(strip.Color(0,0,127), 20);      // blue
  colorWipe(strip.Color(0,0,0), 20);        // black

  // Color sparkles
  dither(strip.Color(0,127,127), 50);       // cyan, slow
  dither(strip.Color(0,0,0), 15);           // black, fast
  dither(strip.Color(127,0,127), 50);       // magenta, slow
  dither(strip.Color(0,0,0), 15);           // black, fast
  dither(strip.Color(127,127,0), 50);       // yellow, slow
  dither(strip.Color(0,0,0), 15);           // black, fast

  // Back-and-forth lights
  scanner(127,0,0, 10);        // red, slow
  scanner(0,127,0, 10);        // green, fast
  scanner(0,0,127, 10);        // blue, fast
  scanner(0,127,127, 10);
  scanner(127,0,127, 10);
  scanner(127,127,0, 10);

  // Wavy ripple effects
  //wave(strip.Color(127,0,0), 4, 20);        // candy cane
  //wave(strip.Color(0,0,100), 2, 40);        // icy

  // make a pretty rainbow cycle!
  rainbowCycle(5);  // make it go through the cycle fairly fast

  // Clear strip data before start of next effect
  for (int i=0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, 0);
  }
  
}

//runs a single challenge, called after start button is pressed
void game(){
  
  //consider doing tone stuff. For now, just go by LEDS
  setAll(0); 
  
  long start = 0;
  long wait = 0;
  
  long temp = 0;
  
  byte player1score = 255;
  byte player2score = 255;
  
  //create a random start
  byte startcounts = random(2,5);
  byte startdelay = random(0,400);
  
  //pulse yellow strips startcounts time with a final delay of start delay
  //if player press button, quite game for foul
  for(int i=0; i< startcounts; i++) {
    start=millis();
    while((millis() - start) < 900){ //delay(900);
      if((digitalRead(PLAYER1BUTTON) == LOW) || (digitalRead(PLAYER2BUTTON) == LOW)){
        setAll(strip.Color(127,0,0)); //red
        noTone(SPEAKER);
        tone(SPEAKER,NOTE_A3,2000); 
        delay(2000);
        return;
      }
    }
   tone(SPEAKER,NOTE_A4); 
    setAll(strip.Color(127,95,0)); //yellow
    start=millis();
    while((millis() - start) < 900){ //delay(900);
      if((digitalRead(PLAYER1BUTTON) == LOW) || (digitalRead(PLAYER2BUTTON) == LOW)){
        setAll(strip.Color(127,0,0)); //red
        noTone(SPEAKER);
        tone(SPEAKER,NOTE_A3,2000); 
        delay(2000);
        return;
      }
    }
    noTone(SPEAKER); 
    setAll(0); //off
  }
  
  start=millis(); 
  while((millis() - start) < (700 + startdelay)){ //delay(900);
      if((digitalRead(PLAYER1BUTTON) == LOW) || (digitalRead(PLAYER2BUTTON) == LOW)){
        setAll(strip.Color(127,0,0)); //red
        noTone(SPEAKER);
        tone(SPEAKER,NOTE_A3,2000); 
        delay(2000);
        return;
      }
    }

  //start!
  
  //make sure no one is cheating by holding down the button
  if((digitalRead(PLAYER1BUTTON) == LOW) || (digitalRead(PLAYER2BUTTON) == LOW)){
    setAll(strip.Color(127,0,0)); //red
    noTone(SPEAKER);
    tone(SPEAKER,NOTE_A3,2000); 
    delay(2000);
  }
  else {
    tone(SPEAKER,NOTE_A5,990);
    setAll(strip.Color(0,127,0)); //green
    digitalWrite(PLAYER1LED, HIGH);
    digitalWrite(PLAYER2LED, HIGH);
    //loop, reading buttons and setting leds up the strip
    //start counting outside loop once, then always start counting before setting colors
    //it looks messy, but it takes into account the time it takes for the LED strip library 
    //to function. 
    start = millis();
    //strip.setPixelColor(0, 0, 0, 127); //blue
    //strip.setPixelColor((LASTPIXEL - 0), 0, 0, 127); //blue
    //strip.show();
    
    //continue on to loop
    for(int i=0; i<NUMPIXELS/2; i++){
      //loop the rest of the time reading the buttons
      while((millis() - start) < 33) {
        
        //see if a button is pushed and record the score
        //also set LED since it will be skipped below
        if(player1score == 255 && (digitalRead(PLAYER1BUTTON) == LOW)){ //player hit the button
          strip.setPixelColor(i, 0, 0, 127); //blue
          player1score = i;
        }
        if(player2score == 255 && (digitalRead(PLAYER2BUTTON) == LOW)){ //player hit the button
          strip.setPixelColor((LASTPIXEL - i), 0, 0, 127); //blue
          player2score = i;
        }
      }
      //start counting here
      start = millis();
      
      //lit up this round's LED
      if(player1score == 255) 
        strip.setPixelColor(i, 0, 0, 127); //blue
      if(player2score == 255) 
        strip.setPixelColor((LASTPIXEL - i), 0, 0, 127); //blue
      strip.show();
    }
  
    delay(800);
    //game over, clear the rest of the green pixels and flash the winner
    for(int i=0; i<NUMPIXELS/2; i++){
      if((player1score == 255) || (i > player1score))
        strip.setPixelColor(i, 0, 0, 0); //off
      if((player2score == 255) || (i > player2score))
        strip.setPixelColor((LASTPIXEL - i), 0, 0, 0); //off
    }
    
    //also need to clear first pixel if no one played
    if(player1score == 255)
      strip.setPixelColor(0, 0, 0, 0); //off
    if(player2score == 255)
      strip.setPixelColor(LASTPIXEL, 0, 0, 0); //off
    
    //push strip update  
    strip.show();
    
    //flash the winner or both if tie
    //flashes player button and their score on the LED strip
    if(player1score < player2score){
      digitalWrite(PLAYER2LED, LOW);
      for(int i = 0; i < 5; i++){
        digitalWrite(PLAYER1LED, LOW);
        for(int y = 0; y <= player1score; y++)
          strip.setPixelColor(y, 0, 0, 127); //blue
        strip.show();
        delay(800);
        digitalWrite(PLAYER1LED, HIGH);
        for(int y = 0; y <= player1score; y++)
          strip.setPixelColor(y, 0, 0, 0); //off
        strip.show();
        delay(800);
      }
    }
    else if(player1score > player2score){
      digitalWrite(PLAYER1LED, LOW);
      for(int i = 0; i < 5; i++){
        digitalWrite(PLAYER2LED, LOW);
        for(int y = 0; y <= player2score; y++)
          strip.setPixelColor((LASTPIXEL - y), 0, 0, 127); //blue
        strip.show();
        delay(800);
        digitalWrite(PLAYER2LED, HIGH);
        for(int y = 0; y <= player2score; y++)
          strip.setPixelColor((LASTPIXEL - y), 0, 0, 0); //off
        strip.show();
        delay(800);
      }
    }
    else if((player1score == player2score) && (player1score != 255)){
      for(int i = 0; i < 5; i++){
        digitalWrite(PLAYER1LED, LOW);
        digitalWrite(PLAYER2LED, LOW);
        for(int y = 0; y <= player2score; y++){
          strip.setPixelColor((LASTPIXEL - y), 0, 0, 127); //blue
          strip.setPixelColor(y, 0, 0, 127); //blue
        }
        strip.show();
        delay(800);
        digitalWrite(PLAYER1LED, HIGH);
        digitalWrite(PLAYER2LED, HIGH);
        for(int y = 0; y <= player2score; y++){
          strip.setPixelColor((LASTPIXEL - y), 0, 0, 0); //off
          strip.setPixelColor(y, 0, 0, 0); //off
        }
        strip.show();
        delay(800);
      }
    }
    else{
      setAll(strip.Color(127,0,0)); //red
      delay(2000);
    }
  }
  setAll(0); //off
  digitalWrite(PLAYER1LED, LOW);
  digitalWrite(PLAYER2LED, LOW);
  
  //delay(3000);  
    
}


void setAll(uint32_t c){
  
  for (int i=0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
  }
  strip.show();
}






//**************************************************************//
//***********Adafruit LED belt code for screensaver*************//
//**************************************************************//




// Cycle through the color wheel, equally spaced around the belt
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for (j=0; j < 384 * 5; j++) {     // 5 cycles of all 384 colors in the wheel
    for (i=0; i < strip.numPixels(); i++) {
      // tricky math! we use each pixel as a fraction of the full 384-color
      // wheel (thats the i / strip.numPixels() part)
      // Then add in j which makes the colors go around per pixel
      // the % 384 is to make the wheel cycle around
      strip.setPixelColor(i, Wheel(((i * 384 / strip.numPixels()) + j) % 384));
    }
    strip.show();   // write all the pixels out
    myDelay(wait);
  }
}

// fill the dots one after the other with said color
// good for testing purposes
void colorWipe(uint32_t c, uint8_t wait) {
  int i;

  for (i=0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
      myDelay(wait);
  }
}

// Chase a dot down the strip
// good for testing purposes
void colorChase(uint32_t c, uint8_t wait) {
  int i;

  for (i=0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, 0);  // turn all pixels off
  }

  for (i=0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, c); // set one pixel
      strip.show();              // refresh strip display
      myDelay(wait);               // hold image for a moment
      strip.setPixelColor(i, 0); // erase pixel (but don't refresh yet)
  }
  strip.show(); // for last erased pixel
}

// An "ordered dither" fills every pixel in a sequence that looks
// sparkly and almost random, but actually follows a specific order.
void dither(uint32_t c, uint8_t wait) {

  // Determine highest bit needed to represent pixel index
  int hiBit = 0;
  int n = strip.numPixels() - 1;
  for(int bit=1; bit < 0x8000; bit <<= 1) {
    if(n & bit) hiBit = bit;
  }

  int bit, reverse;
  for(int i=0; i<(hiBit << 1); i++) {
    // Reverse the bits in i to create ordered dither:
    reverse = 0;
    for(bit=1; bit <= hiBit; bit <<= 1) {
      reverse <<= 1;
      if(i & bit) reverse |= 1;
    }
    strip.setPixelColor(reverse, c);
    strip.show();
    myDelay(wait);
  }
  myDelay(250); // Hold image for 1/4 sec
}

// "Larson scanner" = Cylon/KITT bouncing light effect
void scanner(uint8_t r, uint8_t g, uint8_t b, uint8_t wait) {
  int i, j, pos, dir;

  pos = 0;
  dir = 1;

  for(i=0; i<((strip.numPixels()-1) * 2); i++) {
    // Draw 5 pixels centered on pos.  setPixelColor() will clip
    // any pixels off the ends of the strip, no worries there.
    // we'll make the colors dimmer at the edges for a nice pulse
    // look
    strip.setPixelColor(pos - 2, strip.Color(r/4, g/4, b/4));
    strip.setPixelColor(pos - 1, strip.Color(r/2, g/2, b/2));
    strip.setPixelColor(pos, strip.Color(r, g, b));
    strip.setPixelColor(pos + 1, strip.Color(r/2, g/2, b/2));
    strip.setPixelColor(pos + 2, strip.Color(r/4, g/4, b/4));

    strip.show();
    myDelay(wait);
    // If we wanted to be sneaky we could erase just the tail end
    // pixel, but it's much easier just to erase the whole thing
    // and draw a new one next time.
    for(j=-2; j<= 2; j++) 
        strip.setPixelColor(pos+j, strip.Color(0,0,0));
    // Bounce off ends of strip
    pos += dir;
    if(pos < 0) {
      pos = 1;
      dir = -dir;
    } else if(pos >= strip.numPixels()) {
      pos = strip.numPixels() - 2;
      dir = -dir;
    }
  }
}

// Sine wave effect
#define PI 3.14159265
void wave(uint32_t c, int cycles, uint8_t wait) {
  float y;
  byte  r, g, b, r2, g2, b2;

  // Need to decompose color into its r, g, b elements
  g = (c >> 16) & 0x7f;
  r = (c >>  8) & 0x7f;
  b =  c        & 0x7f; 

  for(int x=0; x<(strip.numPixels()*5); x++)
  {
    for(int i=0; i<strip.numPixels(); i++) {
      y = sin(PI * (float)cycles * (float)(x + i) / (float)strip.numPixels());
      if(y >= 0.0) {
        // Peaks of sine wave are white
        y  = 1.0 - y; // Translate Y to 0.0 (top) to 1.0 (center)
        r2 = 127 - (byte)((float)(127 - r) * y);
        g2 = 127 - (byte)((float)(127 - g) * y);
        b2 = 127 - (byte)((float)(127 - b) * y);
      } else {
        // Troughs of sine wave are black
        y += 1.0; // Translate Y to 0.0 (bottom) to 1.0 (center)
        r2 = (byte)((float)r * y);
        g2 = (byte)((float)g * y);
        b2 = (byte)((float)b * y);
      }
      strip.setPixelColor(i, r2, g2, b2);
    }
    strip.show();
    myDelay(wait);
  }
}

/* Helper functions */

//Input a value 0 to 384 to get a color value.
//The colours are a transition r - g - b - back to r

uint32_t Wheel(uint16_t WheelPos)
{
  byte r, g, b;
  switch(WheelPos / 128)
  {
    case 0:
      r = 127 - WheelPos % 128; // red down
      g = WheelPos % 128;       // green up
      b = 0;                    // blue off
      break;
    case 1:
      g = 127 - WheelPos % 128; // green down
      b = WheelPos % 128;       // blue up
      r = 0;                    // red off
      break;
    case 2:
      b = 127 - WheelPos % 128; // blue down
      r = WheelPos % 128;       // red up
      g = 0;                    // green off
      break;
  }
  return(strip.Color(r,g,b));
}


void myDelay(long time) {
  
  long start = millis();
  
  while((millis() - start) < time){
   if(digitalRead(STARTBUTTON) == LOW){
     digitalWrite(STARTLED, HIGH);
     game();
   }
     
   if(millis() - lastBlink > 250){
     digitalWrite(STARTLED, !digitalRead(STARTLED));
     lastBlink = millis();
   }
  }
  
}














