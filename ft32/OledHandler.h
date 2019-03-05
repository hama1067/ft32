/*
 * OledHandler.h
 *
 *  Created on: Nov 06, 2018
 *      Author: Panic S.
 *
 *  description:   
 */

#ifndef OLEDHANDLER_H
#define OLEDHANDLER_H

#include <Adafruit_GFX.h>
#include "Adafruit_SSD1306.h"
#include <SPI.h>
#include <Wire.h>

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2

  /*
  *   display Initialisation
  */
  void InitOled(Adafruit_SSD1306 display);
  
  /*
  *   display Verbindungsaufbau while login
  */
  void printVerbindungsaufbau(Adafruit_SSD1306 display);
  
  /*
  *   display Verbunden after login
  */
  void printVerbunden(Adafruit_SSD1306 display);
  
  /*
  *   get the ip address of the server
  *   and print on Oled e.g.: 192.168.4.1
  */
  void printLoginData(Adafruit_SSD1306 display, String pIP, String pPasswort, String pssid);

  void printConnectionStatus(Adafruit_SSD1306 display, String pIP, String pSsid);
  
  /*
  *   print Screensaver
  *  
  */
  //  void printScreensave(Adafruit_SSD1306 display);
  
  void printOledMessage(Adafruit_SSD1306 display, String message);

#endif
