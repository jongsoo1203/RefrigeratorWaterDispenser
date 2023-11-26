const int relay = 7;
IRrecv IR(3);

// different waterbottles
const int LoreWHydroFlask = 20;

void setup() {
  // put your setup code here, to run once:
  pinMode(relay, OUTPUT);
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

    // When number 1 is recieved 
    if (IR.decodedIRData.decodedRawData == 0xE916FF00) {
      digitalWrite(relay, HIGH); // Connects the external wires
      delay(getMilli(LoreWHydroFlask)); // Pour the water for .... but use milliseconds
      digitalWrite(relay, LOW); // Disconnects the external wires
    }
    // When number 2 is recieved 
    if (IR.decodedIRData.decodedRawData == 0xE619FF00) {
      digitalWrite(relay, HIGH); // Connects the external wires
      delay(getMilli(......)); // Pour the water for .... but use milliseconds
      digitalWrite(relay, LOW); // Disconnects the external wires
    }
    delay(1500);
    IR.resume();
  }
}

int getMilli(int seconds) {
  return seconds * 1000; // Canvert second in milliseconds
}