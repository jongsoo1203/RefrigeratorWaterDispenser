// Error Doc:
// Error#01 = failed to record individual water 
// Error#02 = failed to push total consumption on database
// Error#03 = failed to generate date or hour due to connection issue


// Note to self: compile with ESP32-WROOM-DA with Baud Rate = 115200

// ******* MODIFY FOR NEW WATER BOTTLE (USE OZ) *******
const float LoreWHydroFlask = 24;
const float LaciHydroFlask = 32;
// *****************************************

const char* textArray1[] = {
  "Press 1 for",
  "Press 2 for",
  "Press 0 for",
  "Press # for",
  "Press * for"
};
const char* textArray2[] = {
  "Lorenzo's Bottle",
  "Laci's Bottle",
  "Unlimited Fill",
  "Custom Oz Fill",
  "Flow Menu"
};

// ************************************
#include <ESP32Firebase.h>
#include <IRremote.h>
#include <time.h>
#include <LCD_I2C.h>
#include <Arduino.h>
#include <Wire.h>
#include <EEPROM.h>
#include <math.h>
#include <esp_sleep.h>

#define _SSID "IoT"          // Your WiFi SSID
#define _PASSWORD "BigKhauna69"      // Your WiFi Password
#define REFERENCE_URL "waterdispenserarduino-default-rtdb.firebaseio.com"  // Your Firebase project reference url
#define uS_TO_S_FACTOR 1000000ULL

struct tm timeinfo;
IRrecv IR(27);
Firebase firebase(REFERENCE_URL);
LCD_I2C lcd(0x27, 16, 2);

const int buzzer = 13;
const int relay = 26;
unsigned long lcdUpdateTime = 0;
int currentTextIndex1,currentTextIndex2  = 0;
const int motionPin = 25;
unsigned long lastMotionTime = 0;
const unsigned long motionDelay = 150000; // 2.5 minutes to screen off 

// wake up timer selection
int wakeHour = 0;
int wakeMinute = 0;
int wakeSeconds = 0;

float foreverTotal = 0;
bool alreadyDoneReset = false;
uint64_t microSecondsUntilNextMidnight = 0;

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  pinMode(buzzer, OUTPUT);
  pinMode(relay, OUTPUT);
  pinMode(motionPin, INPUT);
  // To get the input from the remote control
  IR.enableIRIn();
  lcd.begin();
  lcd.backlight();
  Wire.begin();
  delay(100);

  // Connect to WiFi
  //Serial.print("Connecting to: ");
  lcd.println("Connecting WIFI ");
  lcd.setCursor(0,1);

  Serial.println(_SSID);
  WiFi.begin(_SSID, _PASSWORD);

  int overflowCount = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    //Serial.print("-");
    lcd.print("-");
    overflowCount+= 1;
    if (overflowCount >= 16) {
      ESP.restart();
    }
  }

  // Initialize time with NTP
  configTime(-5 * 3600, 0, "pool.ntp.org", "time.nist.gov");
  
  //Serial.println("Waiting for time");
  while (!getLocalTime(&timeinfo)) {
    delay(300);
  }
}

void loop() {
  updateLCD();
  checkIRInput();
  motionDetection();

  // time_t now = mktime(&timeinfo);
  // struct tm midnightTime = timeinfo;
  // midnightTime.tm_hour = wakeHour;
  // midnightTime.tm_min = wakeMinute;
  // midnightTime.tm_sec = wakeSeconds;
  // time_t midnight = mktime(&midnightTime);
  // long timeToMidnight = difftime(midnight, now);

  // //Serial.println(String(timeToMidnight));

  // if (abs(timeToMidnight) <= 60) {
  //   lcd.clear();
  //   lcd.print("Resetting...");
  //   lcd.setCursor(0, 1);
  //   lcd.print("Please Wait");
  //   // The current time is within the range 00:00 to 00:01
  //   // Execute Reset operation for website purpose 
  //   if (getLocalTime(&timeinfo)) {
  //     if (firebase.setInt("/records/" + formatDate(&timeinfo) + "/lorenzo/" + formatTime(&timeinfo), 0) && firebase.setInt("/records/" + formatDate(&timeinfo) + "/laci/" + formatTime(&timeinfo), 0)) {
  //       foreverTotal = firebase.getFloat("/records/forever_consumption");
  //       foreverTotal += LoreWHydroFlask;
  //       if (firebase.setFloat("/records/forever_consumption", foreverTotal)) {
  //         //Serial.println("Firebase Push Successful");
  //         lcd.clear();
  //         lcd.setCursor(0, 0);
  //         lcd.print("Reset Success");
  //         delay(2000);
  //       } else {
  //         lcd.print("Pushed Failed");
  //         lcd.setCursor(0, 1);
  //         lcd.print("Error#02");
  //         delay(2000);
  //       }
  //     } else {
  //       lcd.print("Pushed Failed");
  //       lcd.setCursor(0, 1);
  //       lcd.print("Error#01");
  //       delay(2000);
  //     }
  //   }
  //   lcd.clear();
  //   lcd.print("Resetting...");
  //   lcd.setCursor(0, 1);
  //   lcd.print("Please Wait");
  //   delay(60000);
  //   ESP.restart();
  // }
  if (WiFi.status() != WL_CONNECTED) {
    ESP.restart();
  }
}

void motionDetection() {
  int motionState = digitalRead(motionPin); // Read sensor state
  //Serial.println(motionState);
  if (motionState == HIGH /*&& !lcdResetFlag*/) {
    //Serial.println("Motion detected!");
    //digitalWrite(LCDLight, LOW); // Activate display when motion is detected
    lastMotionTime = millis();// Record the time of the last motion detection
    lcd.backlight();
    lcd.display();
    //lcdResetFlag = true;
  } else {
    //Serial.println("No motion detected.");
    // Check if the elapsed time since the last motion is greater than the delay
    if (millis() - lastMotionTime > motionDelay) {
      //Serial.println("Going to sleep..."); 
      //lcdResetFlag = false;
      lcd.noBacklight();
      lcd.noDisplay(); // De-activate display when motion is not detected
      //Serial.flush(); 
      //esp_deep_sleep_start();
      //esp_light_sleep_start();
    }
  }
}

void checkIRInput() {
  // Update the current date and time
  float ozNew;
  bool recieved = false;
  if (IR.decode()) {
    // Convert the signal value to a number
    Serial.println(IR.decodedIRData.decodedRawData, HEX);
    // Remote input cases
    switch (IR.decodedIRData.decodedRawData) {
      case 0xE916FF00: // 1 my water
        doubleBeepSound();
        if (getLocalTime(&timeinfo)) {
          lcd.clear();
          lcd.print("Loading...");
          flowing(getSeconds(LoreWHydroFlask), LoreWHydroFlask);
          lcd.print("Loading...");
          if (firebase.setInt("/records/" + formatDate(&timeinfo) + "/lorenzo/" + formatTime(&timeinfo), LoreWHydroFlask)){
            foreverTotal = firebase.getFloat("/records/forever_consumption");
            foreverTotal += LoreWHydroFlask;
            if (firebase.setFloat("/records/forever_consumption", foreverTotal)) {
              //Serial.println("Pushed Success");
              lcd.setCursor(0, 1);
              lcd.print("Pushed Success");
              delay(2000);
            } else {
              Serial.println("Pushed Failed Error#02");
              lcd.print("Pushed Failed");
              lcd.setCursor(0, 1);
              lcd.print("Error#02");
              delay(2000);
            }
          } else {
            Serial.println("Pushed Failed Error#01");
            lcd.print("Pushed Failed");
            lcd.setCursor(0, 1);
            lcd.print("Error#01");
            delay(2000);
          }
        }
        break;
      case 0xE619FF00: // 2 laci water
        doubleBeepSound();
        if (getLocalTime(&timeinfo)) {
          lcd.clear();
          lcd.print("Loading...");
          flowing(getSeconds(LaciHydroFlask), LaciHydroFlask);
          lcd.print("Loading...");
          if (firebase.setInt("/records/" + formatDate(&timeinfo) + "/laci/" + formatTime(&timeinfo), LaciHydroFlask)){
            foreverTotal = firebase.getFloat("/records/forever_consumption");
            foreverTotal += LaciHydroFlask;
            if (firebase.setFloat("/records/forever_consumption", foreverTotal)) {
              //Serial.println("Pushed Success");
              lcd.setCursor(0, 1);
              lcd.print("Pushed Success");
              delay(2000);
            } else {
              Serial.println("Pushed Failed Error#02");
              lcd.print("Pushed Failed");
              lcd.setCursor(0, 1);
              lcd.print("Error#02");
              delay(2000);
            }
          } else {
            Serial.println("Pushed Failed Error#01");
            lcd.print("Pushed Failed");
            lcd.setCursor(0, 1);
            lcd.print("Error#01");
            delay(2000);
          }
        }
        break;
      
      case 0xAD52FF00: // 0
        singleBeepSound();
        digitalWrite(relay, !digitalRead(relay));
        break;
      case 0xB54AFF00: // # custom fill
        //Serial.println("# RECIEVED");
        // increase performance and user expirience but worst memory 
        ozNew = customFilling();
        if (ozNew != 0) {
          // assign water consumprio to the person
          lcd.clear();
          lcd.print("Record it ?");
          lcd.setCursor(0, 1);
          lcd.print("# to cancel");
          // loop wait for user input
          while (!recieved) {
            if (IR.decode()) {
              if (IR.decodedIRData.decodedRawData == 0xBF40FF00) { // if ok is pressed
                doubleBeepSound();
                lcd.clear();
                lcd.print("Press ur number");
                lcd.setCursor(0, 1);
                lcd.print("to record data..");
                while (!recieved) {
                  if (IR.decode()) {
                    switch (IR.decodedIRData.decodedRawData) {
                      case 0xE916FF00: // 1 my water
                        doubleBeepSound();
                        lcd.clear();
                        lcd.print("Loading...");
                        if (firebase.setInt("/records/" + formatDate(&timeinfo) + "/lorenzo/" + formatTime(&timeinfo), ozNew)){
                          foreverTotal = firebase.getFloat("/records/forever_consumption");
                          foreverTotal += ozNew;
                          firebase.setFloat("/records/forever_consumption", foreverTotal);

                          Serial.println("Pushed Success");
                          lcd.setCursor(0, 1);
                          lcd.print("Pushed Success");
                          delay(2000);
                        } else {
                          Serial.println("Pushed Failed Error#01");
                          lcd.print("Pushed Failed");
                          lcd.setCursor(0, 1);
                          lcd.print("Error#01");
                          delay(2000);
                        }
                        recieved = true;
                        break;
                      case 0xE619FF00: // 2 laci water
                        doubleBeepSound();
                        lcd.clear();
                        lcd.print("Loading...");
                        if (firebase.setInt("/records/" + formatDate(&timeinfo) + "/lorenzo/" + formatTime(&timeinfo), ozNew)){
                          foreverTotal = firebase.getFloat("/records/forever_consumption");
                          foreverTotal += ozNew;
                          firebase.setFloat("/records/forever_consumption", foreverTotal);

                          //Serial.println("Pushed Success");
                          lcd.setCursor(0, 1);
                          lcd.print("Pushed Success");
                          delay(2000);
                        } else {
                          //Serial.println("Pushed Failed Error#01");
                          lcd.print("Pushed Failed");
                          lcd.setCursor(0, 1);
                          lcd.print("Error#01");
                          delay(2000);
                        }
                        recieved = true;
                        break;
                    }
                    delay(500);
                    IR.resume();
                  }
                }
              }
              // if # we brake and exit the loop
              else if (IR.decodedIRData.decodedRawData == 0xB54AFF00) {
                doubleBeepSound();
                lcd.clear();
                recieved = true;
                break;
              }
              delay(200);
              IR.resume();
            }
          }
        }
        break;
      
      
      case 0xBD42FF00: // * menu 
        flowMenu();
        break;
    }
    delay(100);
    IR.resume();
  }
}

void flowMenu() {
  doubleBeepSound();
  lcd.clear();
  lcd.setCursor(0, 0);
  float SecForOunce = firebase.getFloat("/flow");
  float flow = SecForOunce; // redundancy for any change check before exiting
  lcd.print("Flow rate: " + String(SecForOunce));
  bool check = true;
  while (check) {
    if (IR.decode()) {
      // Remote input cases
      switch (IR.decodedIRData.decodedRawData) {
        case 0xB946FF00: // up button
          singleBeepSound();
          flow += 0.01;
          lcd.clear();
          lcd.print("Flow rate: " +  String(flow));
          break;
        case 0xEA15FF00: // down button
          singleBeepSound();
          flow -= 0.01;
          lcd.clear();
          lcd.print("Flow rate: " +  String(flow));
          break;
        case 0xBF40FF00: // OK when done
          doubleBeepSound();
          doubleBeepSound();
          if (SecForOunce != flow){
            if (firebase.setFloat("/flow", flow)) {
                lcd.clear();
                //Serial.println("Pushed Success");
                lcd.print("Pushed Success");
                delay(2000);
            } else {
              //Serial.println("Pushed Failed Error#02");
              lcd.clear();
              lcd.print("Pushed Failed");
              lcd.setCursor(0, 1);
              lcd.print("Error#02");
              delay(2000);
            }
            lcd.clear();
            //Serial.println("New flow rate set ");
            lcd.print("New flow rate");
            lcd.setCursor(0, 1);
            lcd.print("stored");
            delay(1000);
          }
          check = false;
          break;
      }
      delay(50);
      IR.resume();
    }
  }
}

void updateLCD() {
  if (millis() - lcdUpdateTime >= 3000) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(textArray1[currentTextIndex1]);

    currentTextIndex1 = (currentTextIndex1 + 1) % (sizeof(textArray1) / sizeof(textArray1[0]));  // Cycle through textArray
    lcd.setCursor(0, 1);
    lcd.print(textArray2[currentTextIndex2]);
    currentTextIndex2 = (currentTextIndex2 + 1) % (sizeof(textArray2) / sizeof(textArray2[0]));
    lcdUpdateTime = millis();
  }
}

void clearSecondLine() {
  lcd.setCursor(0, 1);
  lcd.print("                ");  // Print spaces to clear the second line
}

float customFilling() {
  doubleBeepSound();

  const char* unitMeasure[] = {
    "oz",
    "cups",
    "mL"
  };
  // if anything gets added only need to change output 'if'
  int unitMeasureIndex = 0;

  String resultStr = "";
  bool check = true;
  bool panic = false;
  lcd.clear();
  lcd.println("OK=Done   #=Canc");
  IR.resume();
  while (check) {
    if (IR.decode()) {
      // Remote input cases
      switch (IR.decodedIRData.decodedRawData) {
        case 0xE916FF00: // 1
          singleBeepSound();
          resultStr += "1";
          clearSecondLine();
          lcd.setCursor(0, 1);
          lcd.print(resultStr + unitMeasure[unitMeasureIndex]);
          break;
        case 0xE619FF00: // 2
          singleBeepSound();
          resultStr += "2";
          clearSecondLine();
          lcd.setCursor(0, 1);
          lcd.print(resultStr + unitMeasure[unitMeasureIndex]);
          break;
        case 0xF20DFF00: // 3
          singleBeepSound();
          resultStr += "3";
          clearSecondLine();
          lcd.setCursor(0, 1);
          lcd.print(resultStr + unitMeasure[unitMeasureIndex]);
          break;
        case 0xF30CFF00: // 4
          singleBeepSound();
          resultStr += "4";
          clearSecondLine();
          lcd.setCursor(0, 1);
          lcd.print(resultStr + unitMeasure[unitMeasureIndex]);
          break;
        case 0xE718FF00: // 5
          singleBeepSound();
          resultStr += "5";
          clearSecondLine();
          lcd.setCursor(0, 1);
          lcd.print(resultStr + unitMeasure[unitMeasureIndex]);
          break;
        case 0xA15EFF00: // 6
          singleBeepSound();
          resultStr += "6";
          clearSecondLine();
          lcd.setCursor(0, 1);
          lcd.print(resultStr + unitMeasure[unitMeasureIndex]);
          break;
        case 0xF708FF00: // 7
          singleBeepSound();
          resultStr += "7";
          clearSecondLine();
          lcd.setCursor(0, 1);
          lcd.print(resultStr + unitMeasure[unitMeasureIndex]);
          break;
        case 0xE31CFF00: // 8
          singleBeepSound();
          resultStr += "8";
          clearSecondLine();
          lcd.setCursor(0, 1);
          lcd.print(resultStr + unitMeasure[unitMeasureIndex]);
          break;
        case 0xA55AFF00: // 9
          singleBeepSound();
          resultStr += "9";
          clearSecondLine();
          lcd.setCursor(0, 1);
          lcd.print(resultStr + unitMeasure[unitMeasureIndex]);
          break;
        case 0xAD52FF00: // 0
          singleBeepSound();
          resultStr += "0";
          clearSecondLine();
          lcd.setCursor(0, 1);
          lcd.print(resultStr + unitMeasure[unitMeasureIndex]);
          break;
        case 0xBF40FF00: // OK when done
          singleBeepSound();
          lcd.clear();
          //Serial.println("OK recieved");
          lcd.print(resultStr + unitMeasure[unitMeasureIndex] + "Confirmed");
          delay(500);
          check = false;
          break;
        case 0xB54AFF00: // #
          panic = true;
          lcd.clear();
          lcd.print("CANCELLED");
          doubleBeepSound();
          delay(50);
          doubleBeepSound();
          check = false;
          break;
        case 0xBD42FF00: // *
          singleBeepSound();
          resultStr += ".";
          clearSecondLine();
          lcd.setCursor(0, 1);
          lcd.print(resultStr + "oz");
          break;
        case 0xB946FF00: // up button
          break;
        case 0xEA15FF00: // down button
          break;
        case 0xBB44FF00: // left button
          singleBeepSound();
          unitMeasureIndex -= 1;
          if (unitMeasureIndex < 0) {
            unitMeasureIndex = sizeof(unitMeasure) / sizeof(unitMeasure[0]) - 1;
          }
          clearSecondLine();
          lcd.setCursor(0, 1);
          lcd.print(resultStr + unitMeasure[unitMeasureIndex]);
          break;
        case 0xBC43FF00: // right button
          singleBeepSound();
          unitMeasureIndex += 1;
          if (unitMeasureIndex >= sizeof(unitMeasure) / sizeof(unitMeasure[0])) {
            unitMeasureIndex = 0;
          }
          clearSecondLine();
          lcd.setCursor(0, 1);
          lcd.print(resultStr + unitMeasure[unitMeasureIndex]);
          break;
      }
      delay(200);
      IR.resume();
    }
  }
  if (panic == false) {
    //Serial.println(resultStr);
    float resultFloat = resultStr.toFloat(); // transloate to float

    if (strcmp(unitMeasure[unitMeasureIndex], "oz") == 0){ // check unit measures
      flowing(getSeconds(resultFloat), resultFloat);
    } else if (strcmp(unitMeasure[unitMeasureIndex], "cups") == 0) {
      resultFloat = resultFloat*8; // to get oz
      flowing(getSeconds(resultFloat), resultFloat);
    } else if (strcmp(unitMeasure[unitMeasureIndex], "mL") == 0) {
      resultFloat = resultFloat * 0.0338;
      flowing(getSeconds(resultFloat), resultFloat);
    }
    return(resultFloat);
  }
  return 0;
}

String formatTime(struct tm* timeinfo) {
  char formattedTime[10];
  strftime(formattedTime, sizeof(formattedTime), "%H:%M:%S", timeinfo);
  return String(formattedTime);
}

String formatDate(struct tm* timeinfo) {
  char formattedDate[15];
  strftime(formattedDate, sizeof(formattedDate), "%Y-%m-%d", timeinfo);
  return String(formattedDate);
}
// beep beep sound
void doubleBeepSound() {
  digitalWrite(buzzer, HIGH);
  delay(100);
  digitalWrite(buzzer, LOW);
  delay(50);
  digitalWrite(buzzer, HIGH);
  delay(100);
  digitalWrite(buzzer, LOW);
}

void flowing(int bottle, float oz){
  // sequent to pur the water 
  // starting sound
  doubleBeepSound();
  // relay activation
  digitalWrite(relay, HIGH); // Connects the external wires
  displayLoadingBar(bottle, oz); // wait x time
  digitalWrite(relay, LOW); // Disconnects the external wires
  // end sound
  doubleBeepSound();
  lcd.print("!!!!! DONE !!!!!");
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
}

float getSeconds(float oz) {
  return oz * (firebase.getFloat("/flow")); // it takes 2 second to get 1 ounce
}

// loading bar that will last x seconds
void displayLoadingBar(float seconds, float oz) {
  lcd.clear();
  int barLength = 16; // Number of characters in the loading bar
  float delayTime = seconds * 1000.0 / barLength; // Calculate delay time for each step
  lcd.print("Filling " + String(oz) + "oz");
  lcd.setCursor(0, 1); // Move cursor to the second line
  for (int i = 0; i < barLength; ++i) {
    //lcd.print("#");
    lcd.write(0xFF); // full block characheter 
    delay((int)delayTime); // Convert delayTime to an integer before using it in delay()
  }
  lcd.clear();
}

// single beep sound
void singleBeepSound(){
  digitalWrite(buzzer, HIGH);
  delay(100);
  digitalWrite(buzzer, LOW);
}