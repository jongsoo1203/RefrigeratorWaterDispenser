#include <IRremote.h>
#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
const int relay = 7;
const int buzzer = 8;
IRrecv IR(6);

// different waterbottles
const int LoreWHydroFlask = 10;

void setup() {
  // put your setup code here, to run once:
  pinMode(relay, OUTPUT);
  pinMode(buzzer, OUTPUT);
  lcd.begin(16, 2);
  // To get the input from the remote control
  IR.enableIRIn();
  Serial.begin(9600);
}

void loop() {
  // getting the input from the remote control
  if (IR.decode()) {
    // Convert the signal value to a number
    Serial.println(IR.decodedIRData.decodedRawData, HEX);

    // When number 1 is recieved 
    if (IR.decodedIRData.decodedRawData == 0xE916FF00) {
      flowing(LoreWHydroFlask);
    }
    // When number 2 is recieved 
    if (IR.decodedIRData.decodedRawData == 0xE619FF00) {
      
    }
    delay(1500);
    IR.resume();
  }
}

void flowing(int bottle){
  // sequent to pur the water 

  // starting sound
  digitalWrite(buzzer, HIGH);
  delay(100);
  digitalWrite(buzzer, LOW);
  delay(50);
  digitalWrite(buzzer, HIGH);
  delay(100);
  digitalWrite(buzzer, LOW);

  // relay activation
  digitalWrite(relay, HIGH); // Connects the external wires
  displayLoadingBar(bottle); // wait x time
  digitalWrite(relay, LOW); // Disconnects the external wires
}
void displayLoadingBar(int seconds) {
  int barLength = 16; // Number of characters in the loading bar
  int delayTime = seconds * 1000 / barLength; // Calculate delay time for each step
  lcd.print("Loading...");
  lcd.setCursor(0, 1); // Move cursor to the second line
  for (int i = 0; i < barLength; ++i) {
    lcd.print("#");
    delay(delayTime);
  }
  lcd.clear();
}