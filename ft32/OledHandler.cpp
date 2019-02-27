 /*
 * OledHandler.h
 *
 *  Created on: Nov 06, 2018
 *      Author: Panic S.
 *
 *  description:   
 */
   
#include "OledHandler.h"
   

#define LOGO16_GLCD_HEIGHT 16 
#define LOGO16_GLCD_WIDTH  16 
static const unsigned char PROGMEM logo16_glcd_bmp[] =
{ B00000000, B11000000,
  B00000001, B11000000,
  B00000001, B11000000,
  B00000011, B11100000,
  B11110011, B11100000,
  B11111110, B11111000,
  B01111110, B11111111,
  B00110011, B10011111,
  B00011111, B11111100,
  B00001101, B01110000,
  B00011011, B10100000,
  B00111111, B11100000,
  B00111111, B11110000,
  B01111100, B11110000,
  B01110000, B01110000,
  B00000000, B00110000 };

    void InitOled(Adafruit_SSD1306 display)
    {      
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    
    display.setCursor(0,0);
    display.println("HsKA     Fischertechnik HWG!");
    display.display();
     
    };
   

    void printVerbindungsaufbau(Adafruit_SSD1306 display){
     // display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)
      Serial.println("Display Ausgabe Verbinden.............................");
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(WHITE);
      
      display.setCursor(0,0);
      display.println("Verbindungsaufbau...");
      display.display();
    }

    void printVerbunden(Adafruit_SSD1306 display){
      Serial.println("Display Ausgabe Verbunden............................");
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(WHITE);
      
      display.setCursor(0,8);
      display.println("verbunden");
      display.display();
      delay(2000);
    }

    void printLoginData(Adafruit_SSD1306 display, String pIP, String pPasswort, String pssid){
    
    unsigned int i=0;
  
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    
    display.setCursor(0,i);
    display.println("IP-Adresse");
    
    display.setCursor(0,i+8);
    display.println(pIP);
    
    display.setCursor(0,i+16);
    display.println("Passwort:");
    
    display.setCursor(0,i+24);
    display.println(pPasswort);
    
    display.setCursor(0,i+32);
    display.println("SSID:");
    
    display.setCursor(0,i+40);
    display.println(pssid);
    
    //display.setCursor(0,32);
    //display.println("Zeile: 5");
    //display.setCursor(0,40);
    //display.println("Zeile: 6");
    //display.setCursor(64,48);
    //display.println(i);
    //display.setCursor(0,56);
    //display.println("Zeile: 8");
    
    display.display();
    //display.clearDisplay();
     
    }

  /*  void printScreensave(Adafruit_SSD1306 display){};


void testdrawbitmap(const uint8_t *bitmap, uint8_t w, uint8_t h) {
  uint8_t icons[NUMFLAKES][3];
 
  // initialize
  for (uint8_t f=0; f< NUMFLAKES; f++) {
    icons[f][XPOS] = random(display.width());
    icons[f][YPOS] = 0;
    icons[f][DELTAY] = random(5) + 1;
    
    Serial.print("x: ");
    Serial.print(icons[f][XPOS], DEC);
    Serial.print(" y: ");
    Serial.print(icons[f][YPOS], DEC);
    Serial.print(" dy: ");
    Serial.println(icons[f][DELTAY], DEC);
  }

  while (1) {
    // draw each icon
    for (uint8_t f=0; f< NUMFLAKES; f++) {
      display.drawBitmap(icons[f][XPOS], icons[f][YPOS], bitmap, w, h, WHITE);
    }
    display.display();
    delay(200);
    
    // then erase it + move it
    for (uint8_t f=0; f< NUMFLAKES; f++) {
      display.drawBitmap(icons[f][XPOS], icons[f][YPOS], bitmap, w, h, BLACK);
      // move it
      icons[f][YPOS] += icons[f][DELTAY];
      // if its gone, reinit
      if (icons[f][YPOS] > display.height()) {
        icons[f][XPOS] = random(display.width());
        icons[f][YPOS] = 0;
        icons[f][DELTAY] = random(5) + 1;
      }
    }
   }
}
*/
