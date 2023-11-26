#include <IRremote.h>

// The remote-control reader
// Right = 5 v
// Middle = GRD
// Left = pin3

// Remote-Control signal
// 1 = 3910598400 = E916FF00
// 2 = 3860463360 = E619FF00
// 3 = 4061003520 = F20DFF00

IRrecv IR(3);
int ledPin = 8;

void setup() {
  // To get the input from the remote control
  IR.enableIRIn();
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  // getting the input from the remote control
  if (IR.decode()) {
    // Convert the signal value to a number
    Serial.println(IR.decodedIRData.decodedRawData, HEX);
    if (IR.decodedIRData.decodedRawData == 0xE916FF00) {
      digitalWrite(ledPin, HIGH);
    }
    if (IR.decodedIRData.decodedRawData == E619FF00) {
      digitalWrite(ledPin, LOW);
    }
    delay(1500);
    IR.resume();
  }
}
