/*********************************************************************
The LAZY GARDENER!
My simple soil moisture detection and pump activation utility.

Copyright (c) Chris Drumgoole 2016
http://www.cdrum.com
https://github.com/cdrum/lazy-gardener

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

*********************************************************************/

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// Set up temperature sensor (temporary?)
#define ONE_WIRE_BUS 2
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// Initialize OLED Display for arduino board
#define OLED_MOSI   9
#define OLED_CLK   10
#define OLED_DC    11
#define OLED_CS    12
#define OLED_RESET 13
Adafruit_SSD1306 display(OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

#define LOGO16_GLCD_HEIGHT 16 
#define LOGO16_GLCD_WIDTH  16 
#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

/*
 * OLED Display 128x64
 * X: 128 pixels
 * Y: 64 pixels, 8 lines, 8 pixels each
 */
#define DISPLAY_TEMPERATURE_LINE_X 0
#define DISPLAY_TEMPERATURE_LINE_Y 0
#define DISPLAY_SOIL_THRESHOLD_LINE_X 0
#define DISPLAY_SOIL_THRESHOLD_LINE_Y 8
#define DISPLAY_SOIL_MOISTURE_LEVEL_LINE_X 0
#define DISPLAY_SOIL_MOISTURE_LEVEL_LINE_Y 16
#define DISPLAY_PUMP_STATUS_HEADER_LINE_X 4
#define DISPLAY_PUMP_STATUS_HEADER_LINE_Y 26
#define DISPLAY_PUMP_STATUS_VALUE_LINE_X 2
#define DISPLAY_PUMP_STATUS_VALUE_LINE_Y 32
#define DISPLAY_PUMP_RUNNING_HEADER_LINE_X 40
#define DISPLAY_PUMP_RUNNING_HEADER_LINE_Y 24
#define DISPLAY_PUMP_RUNNING_VALUE_LINE_X 40
#define DISPLAY_PUMP_RUNNING_VALUE_LINE_Y 32
#define DISPLAY_PUMP_STATUS_RECT_1X 0
#define DISPLAY_PUMP_STATUS_RECT_1Y 24
#define DISPLAY_PUMP_STATUS_RECT_2X 38
#define DISPLAY_PUMP_STATUS_RECT_2Y 26 // Not actual pixel, but relative to previous coordinate

// General Configuration Constants
#define MOISTTHRESH 500
#define PUMP_DURATION 3 // 3 counts (seconds?)
#define PUMP_WAIT 5 // 5 counts (seconds?)

// Boolean Constants
#define PUMP_OFF 0
#define PUMP_ON 1

// Global Variables
int mostureSensor = 0;
int moisture_threshold = MOISTTHRESH;
int pump_wait_timer = 0; // How long to wait before we test the soil moisture before activating the pump again, Counter for pump activation wait time
int pump_active_duration_timer = PUMP_DURATION; // How long to run the pump for
bool pump_state = PUMP_OFF;
bool pump_active = false;
bool pump_waiting = false;

/* Setup */
void setup()   {                
  Serial.begin(9600);

  // Start up the library (temperature)
  sensors.begin(); 
  
  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC);
  // init done
  
  // Show image buffer on the display hardware.
  // Since the buffer is intialized with an Adafruit splashscreen
  // internally, this will display the splashscreen.
  display.display();
  delay(1000);

  // Clear the buffer.
  display.clearDisplay();

}

/*
 * Function displays current status of pump pn screen
 */
void displayPumpStatusMessage() {

  if (pump_state == PUMP_ON) {
    // Pump is on
    display.setCursor(DISPLAY_PUMP_STATUS_HEADER_LINE_X, DISPLAY_PUMP_STATUS_HEADER_LINE_Y);
    display.print("PUMP:");
    display.setTextSize(2);
    display.setCursor(DISPLAY_PUMP_STATUS_VALUE_LINE_X+4, DISPLAY_PUMP_STATUS_VALUE_LINE_Y);
    display.setTextColor(BLACK, WHITE);
    display.print("ON");
    display.setTextColor(WHITE, BLACK);
    display.setTextSize(1);
    display.setCursor(DISPLAY_PUMP_RUNNING_HEADER_LINE_X, DISPLAY_PUMP_RUNNING_HEADER_LINE_Y);
    display.print("Time to Run: ");
    display.setCursor(DISPLAY_PUMP_RUNNING_VALUE_LINE_X, DISPLAY_PUMP_RUNNING_VALUE_LINE_Y);
    display.print(pump_active_duration_timer);
    display.print(" sec left.");
  } else {
    // Pump is off
    display.setCursor(DISPLAY_PUMP_STATUS_HEADER_LINE_X, DISPLAY_PUMP_STATUS_HEADER_LINE_Y);
    display.print("PUMP:");
    display.setTextSize(2);
    display.setCursor(DISPLAY_PUMP_STATUS_VALUE_LINE_X, DISPLAY_PUMP_STATUS_VALUE_LINE_Y);
    display.setTextColor(BLACK, WHITE);
    display.print("OFF");
    display.setTextColor(WHITE, BLACK);
    display.setTextSize(1);
    if (pump_wait_timer) { // Only show the timer if we have time left. Otherwise we just let it wait until next time the soil moisture falls below the threshold
        display.setCursor(DISPLAY_PUMP_RUNNING_HEADER_LINE_X, DISPLAY_PUMP_RUNNING_HEADER_LINE_Y);
        display.print("Next Check: ");
        display.setCursor(DISPLAY_PUMP_RUNNING_VALUE_LINE_X, DISPLAY_PUMP_RUNNING_VALUE_LINE_Y);
        display.print(pump_wait_timer);
        display.print(" sec left.");        
    }
  }
  
  // Draw a rectangle around pump status indicator
  display.drawRect(DISPLAY_PUMP_STATUS_RECT_1X, DISPLAY_PUMP_STATUS_RECT_1Y, DISPLAY_PUMP_STATUS_RECT_2X, DISPLAY_PUMP_STATUS_RECT_2Y, WHITE);
}

/*
 * Function to toggle the water pump on/off based on command provided
 * Function checks the pump state and toggles as needed
 */
void togglePump(bool toggle_command = PUMP_OFF) {

  // Check what we need to do  
  if (toggle_command == PUMP_ON) { // Command is to turn pump on (or keep it on)
    
    if (pump_state != PUMP_ON) { // Current state is OFF, so let's turn it on
      // Turn it on!
      // ****** CODE TO BE PLACED HERE ****** //
      
      pump_state = PUMP_ON; // Set the state of the pump to on
    } 
  } else { // Command is to turn pump off (or keep it off)
    
    if (pump_state != PUMP_OFF) { // Current state is ON, so let's turn it off
      // Turn it off!
      // ****** CODE TO BE PLACED HERE ****** //
      
      pump_state = PUMP_OFF; // Set the state of the pump to on
    }
  }
}

/* Main Loop */
void loop() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(DISPLAY_TEMPERATURE_LINE_X, DISPLAY_TEMPERATURE_LINE_Y);
  display.print("Temperature: ");
  
  sensors.requestTemperatures(); // Send the command to get temperatures
  
  display.print(sensors.getTempCByIndex(0)); // Why "byIndex"? You can have more than one IC on the same bus. 0 refers to the first IC on the wire
  
  int sensorValue = 1023-analogRead(mostureSensor);

  display.setCursor(DISPLAY_SOIL_THRESHOLD_LINE_X, DISPLAY_SOIL_THRESHOLD_LINE_Y);
  display.print("Soil Threshold: ");
  display.print(moisture_threshold);

  display.setCursor(DISPLAY_SOIL_MOISTURE_LEVEL_LINE_X, DISPLAY_SOIL_MOISTURE_LEVEL_LINE_Y);
  display.print("Soil Moisture: ");
  display.println(sensorValue);
  
  // Display Pump Status At All Times
  displayPumpStatusMessage(); 
  
  if (pump_state == PUMP_ON) {
    // Pump is active
    
    // Check the timer to make sure we shoudld till be running the pump
    if (--pump_active_duration_timer < 0) {
       // Time ran out, so let's stop the pump
       
       // *** STOP PUMP ***
       togglePump(PUMP_OFF);
       
       // Now set the wait state to true and initialize the timer
       pump_waiting = true;
       pump_wait_timer = PUMP_WAIT;
    }
    
  } else {
    // Pump isn't active
    
    // Let's make sure our delay period isn't still counting
    if (pump_waiting) {
      // We're in the wait period. Let's decrement, and move on
      if (--pump_wait_timer <= 0) {
        // Looks like we're eready to try checking again, so let's change the pump_waiting state to false and let the check happen again
        pump_waiting = false;
      }
    } else {
    
      // We don't need to wait, so let's check the moisture level
      if (sensorValue < moisture_threshold) {
        
        // Set the pump active state to true
        pump_active_duration_timer = PUMP_DURATION;
               
        // *** START PUMP ***
        togglePump(PUMP_ON);
      }
    }
  }

  display.setTextSize(1);
   
  // Print all above to screen
  display.display();
  delay(1000);
}





