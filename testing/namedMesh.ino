#include "namedMesh.h"
#include "FastLED.h"

#define MESH_SSID       "IoTMesh"
#define MESH_PASSWORD   "temp"
#define MESH_PORT        5555
String NODE_NAME = "ExampleLights";
Scheduler  mainScheduler;
namedMesh  mesh;
bool debug = true;
// For later use - accept incoming commands only from the server
String privilegedNodes[] = { "server" };

#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001000)
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif
#define DATA_PIN    3
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
#define NUM_LEDS    64
#define DEFAULT_DELAY 8 // 8ms delay ~ 120 fps
#define NUM_PATTERNS 16
CRGB leds[NUM_LEDS];
uint8_t frameDelay = DEFAULT_DELAY;
uint8_t brightness = -1;
uint8_t currentPattern = 0;
uint8_t hue = 0;
uint8_t bpm = 64;
uint8_t numLamps = NUM_LEDS / 2;
CRGB fixedColour = CRGB::White;
CRGB colourArray[32] = {fixedColour};
uint8_t colourArraySize = 1;
CRGBPalette16 palette;
void solid(), solidWithSparks(), tavern(), singlePulse(),
     multiPulse(), strobe(), multiStrobe(), sparkle(),
     runningLights(), singleWipe(), multiWipe(),
     rainbow(), rainbowWithSpark(), rainbowWithColourSpark(),
     confetti(), beatsPerMinute();
typedef void (*PatternList[])();
PatternList patterns = { solid, solidWithSparks, tavern,
            singlePulse, multiPulse, strobe, multiStrobe,
            sparkle, runningLights, singleWipe, multiWipe,
            rainbow, rainbowWithSpark, rainbowWithColourSpark,
            confetti, beatsPerMinute };
            
void initLED() {
    FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
    FastLED.setBrightness(brightness);
}
// Utility Functions
void updateLED() {
    FastLED.show();
    FastLED.delay(1000/frameDelay);
}
void fadeTo(CRGB colour) {
  float colourScale;
    for(int i = 0; i < 256; i++) {
      colourScale = i / 256.0;
        for(int j = 0; j < NUM_LEDS; j++) {
            leds[j] = colourScale * colour;
        }
        updateLED();
    }
}
void wipe(CRGB colour) {
    for(int i = 0; i < NUM_LEDS; i++) {
        leds[i] = colour;
        updateLED();
    }
}
// User Defined Colour Functions
void solid() {
    for(CRGB led : leds) {
        if(led != fixedColour) {
            led = fixedColour;
        }
    }
}
void solidWithSparks() {
    solid();
    addSparks(80, CRGB::White);
    updateLED();
}
void tavern() {
      int increment = NUM_LEDS / numLamps;
      for(int i = 0; i < increment; i++) {
          for(int j = i; j < NUM_LEDS; j += increment) {
              leds[j] = fixedColour;
          }
          FastLED.show();
          FastLED.delay(300000); // Shift LEDs so they are not on for more than 5 min at a time;
          for(int j = i; j < NUM_LEDS; j += increment) {
              leds[j] = CRGB::Black;
          }
          FastLED.show();
      }
}
void singlePulse() {
    fadeTo(fixedColour);
    fadeTo(CRGB::Black);
}
void multiPulse() {
    for(uint8_t i = 0; i < colourArraySize; i++) {
        fadeTo(colourArray[i]);
        fadeTo(CRGB::Black);
    }
}
void strobe() {
    if(frameDelay != 0) frameDelay = 0;
    singlePulse();
}
void multiStrobe() {
    if(frameDelay != 0) frameDelay = 0;
    multiPulse();
}
void sparkle() {
    int i = random(NUM_LEDS);
    leds[i] = fixedColour;
    updateLED();
    leds[i] = CRGB::Black;
}
void runningLights() {
    float colourScale;
    int pos = 0;
    for(int i = 0; i < NUM_LEDS + NUM_LEDS; i++) {
        pos++;
        for(int j = 0; j < NUM_LEDS; j++) {
            colourScale = (quadwave8(i + pos) * 127 + 128) / 255;
            leds[i] = colourScale * fixedColour;
        }
        updateLED();
    }
}
void singleWipe() {
    wipe(fixedColour);
}
void multiWipe() { 
    for(CRGB colour : colourArray) {
        wipe(colour);
    }
}
// Rainbow/Hue Functions
void rainbow() { fill_rainbow(leds, NUM_LEDS, hue++, 7); updateLED();}
void rainbowWithSpark() { rainbow(); addSparks(80, CRGB::White); updateLED();}
void rainbowWithColourSpark() { rainbow(); addSparks(80, fixedColour); updateLED(); }
void addSparks(fract8 percentSpark, CRGB colour) {
    if(random8() < percentSpark) {
        leds[random16(NUM_LEDS)] += colour;
    }
}
void confetti() {
    fadeToBlackBy(leds, NUM_LEDS, 10);
    leds[random16(NUM_LEDS)] += CHSV(hue++ + random8(64), 200, 255);
    updateLED();
}
void beatsPerMinute() {
    palette = PartyColors_p;
    uint8_t beat = beatsin8(bpm, 64, 255);
    for(int i = 0; i < NUM_LEDS; i++) {
        leds[i] = ColorFromPalette(palette, hue + (i + i), beat - hue++ + (i * 10));
    }
    updateLED();
}

void mainFunc() {
    patterns[currentPattern]();
}

Task mainTask(TASK_SECOND, TASK_FOREVER, &mainFunc);

void initMesh() {
   if(debug) mesh.setDebugMsgTypes(ERROR | DEBUG | CONNECTION);
   mesh.init(MESH_SSID, MESH_PASSWORD, &mainScheduler, MESH_PORT);
   mesh.setName(NODE_NAME);
   // TODO
   // Configure message parsing
   mesh.onReceive([](uint32_t from, String &msg) {
      Serial.printf("Received message by id from: %u, %s\n", from, msg.c_str());
  });
   mesh.onReceive([](String &from, String &msg) {
      Serial.printf("Received message by name from: %s, %s\n", from.c_str(), msg.c_str());
  });
  mesh.onChangedConnections([]() {
    Serial.printf("Changed connection\n");
  });
}

void setup() {
  delay(3000);
  if(debug) Serial.begin(115200);
  initMesh();
  initLED();
  mainScheduler.addTask(mainTask);
  mainTask.enable();
}

void loop() {
  mesh.update();
}
