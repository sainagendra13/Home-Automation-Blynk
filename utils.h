#ifndef UTILS_H
#define UTILS_H

#define BLYNK_TEMPLATE_ID "TMPL3PVq_BQDS"
#define BLYNK_TEMPLATE_NAME "Home Automation"
#define BLYNK_AUTH_TOKEN "Your_Auth_Token_Here"

#include <SPI.h>
#include <Ethernet.h>
#include <Wire.h>

// Definitions
#define TEMP A1
#define HEATER 3
#define COOLER 6

#define TANK_MAX 3000
#define TANK_MIN 500



// External objects (tells logs.cpp these exist in the main ino) 
String logToTerminal(String message);

#endif
