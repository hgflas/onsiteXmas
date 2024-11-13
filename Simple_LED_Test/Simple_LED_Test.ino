#include <Adafruit_NeoPixel.h>


/* LED Configuration */
#define LED_PIN     14  // Define pin for LED strip
#define LED_COUNT   12  // Number of LEDs
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  Serial.begin(115200);
  strip.begin();
  strip.setBrightness(50);
  strip.show();
}

void loop() {
  strip.clear();
  Serial.println("Start ... ");
  for(int i=0; i<LED_COUNT; i++) {

    strip.setPixelColor(i, strip.Color(226, 0, 116));
    strip.show();
    delay(500);
  }

}
