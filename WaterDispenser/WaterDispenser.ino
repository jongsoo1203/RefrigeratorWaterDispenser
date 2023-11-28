#include <IRremote.h>
#include <LiquidCrystal.h>

// ******* MODIFY FOR NEW WATER BOTTLE (USE OZ) *******
const float LoreWHydroFlask = 24;
const float LaciHydroFlask = 32;

// *****************************************

const char* textArray1[] = {
  "Press 1 for",
  "Press 2 for",
  "Press 0 for",
  "Press # for"
};
const char* textArray2[] = {
  "Lorenzo's Bottle",
  "Laci's Bottle",
  "Unlimited Fill",
  "Custom Oz Fill"
};

// ************************************


LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
const int relay = 7;
const int buzzer = 8;
IRrecv IR(6);
unsigned long lcdUpdateTime = 0;
unsigned long irCheckTime = 0;
int currentTextIndex1,currentTextIndex2  = 0;
const int LCD = 9;
const int LCDBacklight = 10;
const int motionPin = 13;
unsigned long lastMotionTime = 0;
const unsigned long motionDelay = 300000; // 5 minutes to screen off 
bool lcdResetFlag = false;

void setup() {
  pinMode(relay, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(LCD, OUTPUT);
  pinMode(motionPin, INPUT);
  pinMode(LCDBacklight, OUTPUT);
  digitalWrite(LCDBacklight, HIGH);
  lcd.begin(16, 2);
  // To get the input from the remote control
  IR.enableIRIn();
  Serial.begin(9600);
}

// MAIN
void loop() {
  // getting the input from the remote control
  updateLCD();
  checkIRInput();
  motionDetection();
}

// turn screen off whn not needed
void motionDetection() {
  int motionState = digitalRead(motionPin); // Read sensor state
  
  if (motionState == HIGH && !lcdResetFlag) {
    Serial.println("Motion detected!");
    digitalWrite(LCD, LOW); // Activate display when motion is detected
    digitalWrite(LCDBacklight, HIGH);
    lcdReset();
    lastMotionTime = millis();// Record the time of the last motion detection
    lcdResetFlag = true;
  } else {
    //Serial.println("No motion detected.");
    // Check if the elapsed time since the last motion is greater than the delay
    if (millis() - lastMotionTime > motionDelay) {
      digitalWrite(LCD, HIGH); // De-activate display when motion is detected
      digitalWrite(LCDBacklight, LOW);
      lcdResetFlag = false;
    }
  }
}

void lcdReset(){
  lcd.begin(16, 2);
}

// display 
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

// read input
void checkIRInput() {
  if (IR.decode()) {
    // Convert the signal value to a number
    Serial.println(IR.decodedIRData.decodedRawData, HEX);
    // Remote input cases
    switch (IR.decodedIRData.decodedRawData) {
      case 0xE916FF00: // 1
        flowing(getSeconds(LoreWHydroFlask));
        break;
      case 0xE619FF00: // 2
        flowing(getSeconds(LaciHydroFlask));
        break;
      case 0xB54AFF00: // #
        doubleBeepSound();
        Serial.println("#");
        customFilling();
        break;
      case 0xAD52FF00: // 0
        if (digitalRead(relay) == HIGH) {
          doubleBeepSound();
          digitalWrite(relay, LOW);
        } else {
          doubleBeepSound();
          digitalWrite(relay, HIGH);
        }
        break;
    }
    delay(1500);
    IR.resume();
  }
}

// custom fillin input reading
void customFilling() {
  String resultStr = "";
  bool check = true;
  bool panic = false;
  lcd.clear();
  lcd.print("OK=Done   #=Canc");
  lcd.setCursor(0, 1);
  IR.resume();
  while (check == true) {
    if (IR.decode()) {
      // Remote input cases
      switch (IR.decodedIRData.decodedRawData) {
        case 0xE916FF00: // 1
          singleBeepSound();
          resultStr += "1";
          Serial.println("1");
          lcd.print("1");
          break;
        case 0xE619FF00: // 2
          singleBeepSound();
          resultStr += "2";
          Serial.println("2");
          lcd.print("2");
          break;
        case 0xF20DFF00: // 3
          singleBeepSound();
          resultStr += "3";
          Serial.println("3");
          lcd.print("3");
          break;
        case 0xF30CFF00: // 4
          singleBeepSound();
          resultStr += "4";
          Serial.println("4");
          lcd.print("4");
          break;
        case 0xE718FF00: // 5
          singleBeepSound();
          resultStr += "5";
          Serial.println("5");
          lcd.print("5");
          break;
        case 0xA15EFF00: // 6
          singleBeepSound();
          resultStr += "6";
          Serial.println("6");
          lcd.print("6");
          break;
        case 0xF708FF00: // 7
          singleBeepSound();
          resultStr += "7";
          Serial.println("7");
          lcd.print("7");
          break;
        case 0xE31CFF00: // 8
          singleBeepSound();
          resultStr += "8";
          Serial.println("8");
          lcd.print("8");
          break;
        case 0xA55AFF00: // 9
          singleBeepSound();
          resultStr += "9";
          Serial.println("9");
          lcd.print("8");
          break;
        case 0xAD52FF00: // 0
          singleBeepSound();
          resultStr += "0";
          Serial.println("0");
          lcd.print("0");
          break;
        case 0xBF40FF00: // OK when done
          singleBeepSound();
          lcd.clear();
          Serial.println("OK recieved");
          lcd.print(resultStr + " Oz Confirmed");
          delay(2000);
          check = false;
          break;
        case 0xB54AFF00: // #
          panic = true;
          lcd.clear();
          lcd.print("CANCELLCD");
          doubleBeepSound();
          delay(50);
          doubleBeepSound();
          check = false;
          break;
        }
      delay(500);
      IR.resume();
    }
  }
  if (panic == false) {
    Serial.println(resultStr);
    flowing(getSeconds(resultStr.toFloat()));
  }
}

// fill up 
void flowing(int bottle){
  // sequent to pur the water 
  // starting sound
  doubleBeepSound();
  // relay activation
  digitalWrite(relay, HIGH); // Connects the external wires
  displayLoadingBar(bottle); // wait x time
  digitalWrite(relay, LOW); // Disconnects the external wires
  // end sound
  doubleBeepSound();
  lcd.print("!!!!! DONE !!!!!");
  delay(5000);
  lcd.clear();
}

// single beep sound
void singleBeepSound(){
  digitalWrite(buzzer, HIGH);
  delay(100);
  digitalWrite(buzzer, LOW);
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

// calculate hwo many second its needed based on the oz
float getSeconds(float oz) {
  return oz * 2.0;
}

// loading bar that will last x seconds
void displayLoadingBar(float seconds) {
  lcd.clear();
  int barLength = 16; // Number of characters in the loading bar
  float delayTime = seconds * 1000.0 / barLength; // Calculate delay time for each step
  lcd.print("Filling Up...");
  lcd.setCursor(0, 1); // Move cursor to the second line
  for (int i = 0; i < barLength; ++i) {
    //lcd.print("#");
    lcd.write(0xFF); // full block characheter 
    delay((int)delayTime); // Convert delayTime to an integer before using it in delay()
  }
  lcd.clear();
}