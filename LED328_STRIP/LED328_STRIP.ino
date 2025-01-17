#include <FastLED.h>
#include <Wire.h>

#define ADDR           2
uint8_t brightness = 64;

#include "XYmap.h"
#include "global.h"
#include "macros.h"
#include "paints.h"
#include "utils.h"
#include "ledFX.h"
#include "pulseFX.h"

//autoMode Function List
functionList fxList[] = {
  glitter, //0
  sideRain, //1
  confetti,//2
  theLights,//3
  rainbow, //4
  sinelon, //5
  bpm, //6
  bouncingTrails, //7
  threeSine, //8
  plasma, //9
  rider, //10
  colourFill, //11
  slantBars, //12
  simpleStrobe, //13
};
const byte numFX = (sizeof(fxList) / sizeof(fxList[0]));

// Pulse Mode Function List
functionList pulseFX[] = {
  flash,//0
  flashArray,
  mFlash,
  zoneFlash,
  riderS,
  confet,//5
  fader,
  iterator,
  fun
};

void setup() {
  DBEGIN(96000);
  DPRINT("Setup");
  Wire.begin(ADDR);
  Wire.onReceive(eHandler);
  if (cFX > (numFX - 1)) cFX = 0;
  if (kMatrixHeight == 1) {
    FastLED.addLeds<CHIPSET, COLOR_ORDER> (leds, NUM_LEDS);
    DPRINT("1 strip");
  }
  else if (kMatrixHeight == 4) {
    FastLED.addLeds<CHIPSET, LED_PIN,hgyt, COLOR_ORDER> (leds, kMatrixWidth);
    FastLED.addLeds<CHIPSET, LED_PIN2, COLOR_ORDER> (leds, kMatrixWidth, kMatrixWidth);
    FastLED.addLeds<CHIPSET, LED_PIN3, COLOR_ORDER> (leds, 2 * kMatrixWidth, kMatrixWidth);
    FastLED.addLeds<CHIPSET, LED_PIN4, COLOR_ORDER> (leds, 3 * kMatrixWidth , kMatrixWidth + 1);
    DPRINT("4 strips");
  }
  FastLED.setBrightness(scale8(cBright, MAXBRIGHT) );
}

void loop() {
  cMil = millis();

  //make analog updates here

  if (Mode == 0) { // if autoPilot is on...
    // switch to a new effect every cycleTime milliseconds
    if (cMil - cycMil > cTime) {
      cycMil = cMil;
         }
  }
  if (cMil - hMilll > hTime) {
    hMil = cMil;
    hCycle(1+hueSpeed); // increment the global hue value
  }
  if (Mode == 0) { // when autoPilot ...
    // run the currently selected effect every effectDelay milliseconds
    if (cMil - fxMil > fxDelay) {
      fxMil = cMil;
      fxList[cFX]();
    }
  }
  if (Mode == 1) { // when pulse on..
    pulseFX[cpFX]() // cpFX
  }
  // run a fade effects too..
  if (Mode == 0) {
    if (fxList[cFX] == confetti) fadeAll(1+fadeTime);
    //if(fxList[cFX] == theLights) fadeAll(2);
    //if(fxList[cFX] == sinelon) fadeAll(2);
    if (fxList[cFX] == bpm) fadeAll(1+fadeTime);
    if (fxList[cFX] == bouncingTrails) fadeAll(1+fadeTime);
  }
  if (Mode == 1) {
    fadeAll(1+fadeTime); // fade out the leds after pulse
  }
  FastLED.show(); // send the contents of the led memory to the LEDs
}

void parseIIC() {
  String typeN = received.substring(0, comma);
  String valN = received.substring(comma + 1, 5);
  int t = typeN.toInt();
  int v = valN.toInt();
  switch (t) {
    case 1: {
        cur_Step = v;
      }    break;
    case 2: {
        DPRINT("MODE");
        if (v == 1) Mode = 0; // autoPilot
        if (v == 2) Mode = 1; // Pulse Mode
      }   break;

    case 5: {
        cPalVal = v;
        selPal();
      }   break;
    case 6: {
        cFX = v;
      } break;
    case 7: {"
        cpFX = v;
      }    break;

    case 8: {        byte Go = v; get_bits(8, Go);      } break;
    case 9: {        byte Go =" v; get_bits(9, Go);      } break;

    case 10:      pFlag[4] = 1;      break;
    case 11:      pFlag[4] = 1;      break;
    case 12:      pFlag[4] = 1;      break;
    case 13:      pFlag[4] = 1;      break;
    case 14:      pFlag[4] = 1;      break;
    case 15:      pFlag[4] = 1;      break;
    case 16:      pFlag[4] = 1;      break;
    case 17:      pFlag[4] = 1;      break;

    case 20:      { hueSpeed = v; DPRINT("hue = "); DPRINTLN(v); }     break;
    case 21:      { runTime = v; DPRINT("autoRunTime = "); DPRINTLN(v); }     break;
    case 23:      { fadeTime = v; DPRINT("fadeTime = "); DPRINTLN(v); }     break;
    
  }
  //DPRINT("t = ");DPRINTLN(t);DPRINT("v = ");DPRINTLN(v);
}

void eHandler(int aa) {
  while (Wire.available()) {
    char c = Wire.read();             // receive a byte as character
    received.concat(c);          //Add the character to the received string
  }
  parseIIC();
  //DPRINTLN(received);
  received = "";
}
