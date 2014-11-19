#include <Adafruit_WS2801.h>
#include "SPI.h" // Comment out this line if using Trinket or Gemma
#ifdef __AVR_ATtiny85__
 #include <avr/power.h>
#endif

#define THRESHOLD 500
#define STATE_OFF     0
#define STATE_ON      1
#define STATE_FADEIN  2
#define STATE_FADEOUT 4

#define LOGTIME 9
#define FDELAY 1  // forced delay

uint8_t dataPin  = MOSI;    // Yellow wire on Adafruit Pixels
uint8_t clockPin = SCK;    // Green wire on Adafruit Pixels
uint8_t IR0 = A0;

float t = 0.0f;
float v = 0.0f;
int state = STATE_OFF;
boolean override = false;

int old_sensor = 0;

// Set the first variable to the NUMBER of pixels. 25 = 25 pixels in a row
Adafruit_WS2801 strip = Adafruit_WS2801(50, dataPin, clockPin);

// Optional: leave off pin numbers to use hardware SPI
// (pinout is then specific to each board and can't be changed)
//Adafruit_WS2801 strip = Adafruit_WS2801(25);

// For 36mm LED pixels: these pixels internally represent color in a
// different format.  Either of the above constructors can accept an
// optional extra parameter: WS2801_RGB is 'conventional' RGB order
// WS2801_GRB is the GRB order required by the 36mm pixels.  Other
// than this parameter, your code does not need to do anything different;
// the library will handle the format change.  Examples:
//Adafruit_WS2801 strip = Adafruit_WS2801(25, dataPin, clockPin, WS2801_GRB);
//Adafruit_WS2801 strip = Adafruit_WS2801(25, WS2801_GRB);

void setup() {
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000L)
  clock_prescale_set(clock_div_1); // Enable 16 MHz on Trinket
#endif
  strip.begin();
  strip.show();
  //  Serial.begin(9600);
}


void loop() {
 
  

  //int a = read_sensor();

  //Serial.println(sensor);
  //delay(500);
  /*
    
  
  }*/
  hue_shift(20);
  //old_sensor = sensor;

  

  //delay(100);
}

void hue_shift(uint8_t wait) {
  for (int j = 0; j < 360; j++) {
    //float h = ;
    int sensor = analogRead(A0) > THRESHOLD;
    boolean transit = (state == STATE_FADEIN) || (state == STATE_FADEOUT);
    //boolean on = (state == STATE_ON);
    
    /*if (sensor) v = 1.0f;
    else v = 0.2f;    */
    
    // save fadein or fadeout time
    if ( (
          (!old_sensor &&  sensor) ||
          ( old_sensor && !sensor)
         ) && !transit
       ) 
    {
      
      t = float(millis())/1000;
    } 
    else if ( (old_sensor && sensor) || override ) {
      float x = float(millis())/1000 - t;
      if (v < 1.0f) {        
        v = fadein(x);
        state = STATE_FADEIN;
      } else {
        v = 1.0f;      
        state = STATE_ON;
        if (x < LOGTIME + FDELAY) override = true;
        else { 
          override = false;
          t = float(millis())/1000;
        } 
      }
    }    
    else if (!old_sensor && !sensor) {
      float x = float(millis())/1000 - t;
      if (v > 0.f) {        
        v = fadeout(x);
        state = STATE_FADEOUT;      
      } else {
        v = 0.f;
        state = STATE_OFF;
      }
    }
      
      
    
    
    old_sensor = sensor;
    
    for (int i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, toRGB(j+i, 1.0, v));
    }
    strip.show();
    delay(wait);
  }
}

int read_sensor() {
  //return (analogRead(pin)> THRESHOLD);\
  return 500;
}

float fadein(float x) {
  //if (x > 3.f) return 1.f;
  //else if (x <= 0.f) return 0.f;  
  return log(3*x+1);  
}

float fadeout(float x) {
  //if (x > 3.f) return 1.f;
  //else if (x <= 0.f) return 0.f;  
  return 1.0f - log(3*x+1);  
}

/* Helper functions */

// Create a 24 bit color value from R,G,B
uint32_t Color(byte r, byte g, byte b)
{
  uint32_t c;
  c = r;
  c <<= 8;
  c |= g;
  c <<= 8;
  c |= b;
  return c;
}

uint32_t toRGB(float h, float s, float v) {
  uint32_t r, g, b;
  float rr, gg, bb;
  float p, q, t, f;
  int i;
  
  //h = constrain(h, 0.f, 359.f);
  s = constrain(s, 0.f, 1.0f);
  v = constrain(v, 0.f, 1.0f);
  
  
  if (s == 0) {
    r = g = b = v;
    return Color(r, g, b);
  }
  
  if (h >= 360.f) h = int(h)%359;
  
  h /= 60;
  i = floor(h);
  f = h - i;
  
  p = v * (1.f - s);
  q = v * (1.f - s*f);
  t = v * (1.f - s*(1-f));
  
  switch(i) {
    case 0:
      rr = v;
      gg = t;
      bb = p;
      break;
    case 1:
      rr = q;
      gg = v;
      bb = p;
      break;
    case 2:
      rr = p;
      gg = v;
      bb = t;
      break;
    case 3:
      rr = p;
      gg = q;
      bb = v;
      break;
    case 4:
      rr = t;
      gg = p;
      bb = v;
      break;   
    case 5:
    default:
      rr = v;
      gg = p;
      bb = q;
      break;
  }
  
  r = floor(rr * 255.f);
  g = floor(gg * 255.f);
  b = floor(bb * 255.f);
 
  return Color (r, g, b);  
}

