//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 03_Touchscreen
//##############################################################################################
//# This sketch or program code is based on the example file "On_Off_Button.ino"               #
//# Arduino IDE (File -> Examples -> TFT_eSPI -> Generic -> On_Off_Button).                    #
//# I made a little modification of the program code "On_Off_Button".                          #
//# If this program code does not work properly, please try the original code (On_Off_Button). #
//##############################################################################################

//---------------------------------------- Including Libraries.
#include "FS.h"  //--> // Calibration data is stored in SPIFFS so we need to include it.
#include <SPI.h>
#include <TFT_eSPI.h> // Hardware-specific library
//---------------------------------------- 

//---------------------------------------- 
// This is the file name used to store the touch coordinate calibration data.
// Change the name to start a new calibration.
#define CALIBRATION_FILE "/TouchCalData3"
//---------------------------------------- 

//---------------------------------------- 
// Set REPEAT_CAL to true instead of false to run calibration again, otherwise it will only be done once.
// Repeat calibration if you change the screen rotation.
//
// Set REPEAT_CAL to true if you want to recalibrate. Then Upload the program code.
// If you have finished recalibrating, Set REPEAT_CAL to false,
// then upload the program code (so that the calibration is not repeated).
#define REPEAT_CAL false
//---------------------------------------- 

// Comment out to stop drawing black spots.
#define BLACK_SPOT

// Switch position and size.
#define FRAME_X 100
#define FRAME_Y 64
#define FRAME_W 120
#define FRAME_H 50

// Red zone size.
#define REDBUTTON_X FRAME_X
#define REDBUTTON_Y FRAME_Y
#define REDBUTTON_W (FRAME_W/2)
#define REDBUTTON_H FRAME_H

// Green zone size.
#define GREENBUTTON_X (REDBUTTON_X + REDBUTTON_W)
#define GREENBUTTON_Y FRAME_Y
#define GREENBUTTON_W (FRAME_W/2)
#define GREENBUTTON_H FRAME_H

// Defines the LED PIN.
#define my_LED 27

bool SwitchOn = false;

// Invoke custom library.
TFT_eSPI tft = TFT_eSPI();



//________________________________________________________________________________ touch_calibrate()
void touch_calibrate() {
  uint16_t calData[5];
  uint8_t calDataOK = 0;

  // check file system exists.
  if (!SPIFFS.begin()) {
    Serial.println("Formatting file system");
    SPIFFS.format();
    SPIFFS.begin();
  }

  // check if calibration file exists and size is correct.
  if (SPIFFS.exists(CALIBRATION_FILE)) {
    if (REPEAT_CAL) {
      // Delete if we want to re-calibrate.
      SPIFFS.remove(CALIBRATION_FILE);
    }
    else {
      File f = SPIFFS.open(CALIBRATION_FILE, "r");
      if (f) {
        if (f.readBytes((char *)calData, 14) == 14)
          calDataOK = 1;
        f.close();
      }
    }
  }

  if (calDataOK && !REPEAT_CAL) {
    // calibration data valid.
    tft.setTouch(calData);
  } else {
    // data not valid so recalibrate.
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(20, 0);
    tft.setTextFont(2);
    tft.setTextSize(1);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);

    tft.println("Touch corners as indicated");

    tft.setTextFont(1);
    tft.println();

    if (REPEAT_CAL) {
      tft.setTextColor(TFT_RED, TFT_BLACK);
      tft.println("Set REPEAT_CAL to false to stop this running again!");
    }

    tft.calibrateTouch(calData, TFT_MAGENTA, TFT_BLACK, 15);

    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.println("Calibration complete!");

    // store data.
    File f = SPIFFS.open(CALIBRATION_FILE, "w");
    if (f) {
      f.write((const unsigned char *)calData, 14);
      f.close();
    }
  }
}
//________________________________________________________________________________ 



//________________________________________________________________________________ drawFrame()
void drawFrame() {
  tft.drawRect(FRAME_X, FRAME_Y, FRAME_W, FRAME_H, TFT_BLACK);
}
//________________________________________________________________________________ 



//________________________________________________________________________________ redBtn()
// Draw a red button.
void redBtn() {
  tft.fillRect(REDBUTTON_X, REDBUTTON_Y, REDBUTTON_W, REDBUTTON_H, TFT_RED);
  tft.fillRect(GREENBUTTON_X, GREENBUTTON_Y, GREENBUTTON_W, GREENBUTTON_H, TFT_DARKGREY);
  drawFrame();
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
  tft.setTextDatum(MC_DATUM);
  tft.drawString("ON", GREENBUTTON_X + (GREENBUTTON_W / 2), GREENBUTTON_Y + (GREENBUTTON_H / 2));
  SwitchOn = false;
}
//________________________________________________________________________________ 



//________________________________________________________________________________ greenBtn()
// Draw a green button.
void greenBtn() {
  tft.fillRect(GREENBUTTON_X, GREENBUTTON_Y, GREENBUTTON_W, GREENBUTTON_H, TFT_GREEN);
  tft.fillRect(REDBUTTON_X, REDBUTTON_Y, REDBUTTON_W, REDBUTTON_H, TFT_DARKGREY);
  drawFrame();
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
  tft.setTextDatum(MC_DATUM);
  tft.drawString("OFF", REDBUTTON_X + (REDBUTTON_W / 2) + 1, REDBUTTON_Y + (REDBUTTON_H / 2));
  SwitchOn = true;
}
//________________________________________________________________________________ 



//________________________________________________________________________________ VOID SETUP()
void setup() {
  // put your setup code here, to run once:

  pinMode(26, OUTPUT);
  digitalWrite(26, LOW);

  Serial.begin(115200);
  Serial.println();
  delay(3000);

  pinMode(my_LED, OUTPUT);
  digitalWrite(my_LED, LOW);
  
  tft.init();

  // Set the rotation before we calibrate.
  tft.setRotation(1);

  // call screen calibration.
  touch_calibrate();

  // clear screen.
  tft.fillScreen(TFT_BLUE);

  // Draw button (this example does not use library Button class).
  redBtn();
}
//________________________________________________________________________________ 



//________________________________________________________________________________ VOID LOOP()
void loop() {
  // put your main code here, to run repeatedly:

  uint16_t x, y;

  // See if there's any touch data for us.
  if (tft.getTouch(&x, &y)) {
    // Draw a block spot to show where touch was calculated to be.
    #ifdef BLACK_SPOT
      tft.fillCircle(x, y, 2, TFT_BLACK);
    #endif
    
    if (SwitchOn) {
      if ((x > REDBUTTON_X) && (x < (REDBUTTON_X + REDBUTTON_W))) {
        if ((y > REDBUTTON_Y) && (y <= (REDBUTTON_Y + REDBUTTON_H))) {
          Serial.println("Red btn hit");
          // Turn off the LED.
          digitalWrite(my_LED, LOW);
          redBtn();
        }
      }
    }
    else {  //--> Record is off (SwitchOn == false).
      if ((x > GREENBUTTON_X) && (x < (GREENBUTTON_X + GREENBUTTON_W))) {
        if ((y > GREENBUTTON_Y) && (y <= (GREENBUTTON_Y + GREENBUTTON_H))) {
          Serial.println("Green btn hit");
          // Turn on the LED.
          digitalWrite(my_LED, HIGH);
          greenBtn();
        }
      }
    }
    ///home/ducanh-vinaembedded/Desktop/Platform-IO/essp32_ili9341_test
    //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

    Serial.println(SwitchOn);
  }
}
//________________________________________________________________________________ 
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<