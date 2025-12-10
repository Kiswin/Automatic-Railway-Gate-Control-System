#include <Servo.h>

// --- Configuration ---
const int PIN_SENSOR_ARRIVAL = 2;    // IR Sensor 1 triggers when train arrives
const int PIN_SENSOR_DEPARTURE = 3;  // IR Sensor 2 triggers when train leaves
const int PIN_BUZZER = 8;
const int PIN_SERVO = 9;
const int PIN_LED_RED = 12;          // Danger / Gate Closed
const int PIN_LED_GREEN = 13;        // Safe / Gate Open

// Servo Angles
const int ANGLE_OPEN = 90;
const int ANGLE_CLOSED = 0;

Servo gateServo;

// State tracking
bool isGateClosed = false;

void setup() {
  Serial.begin(9600);
  
  // Pin Setup
  pinMode(PIN_SENSOR_ARRIVAL, INPUT); // Assuming IR sensors usually output LOW when object detected, but implies external pull-up or active output. Using INPUT.
  pinMode(PIN_SENSOR_DEPARTURE, INPUT);
  pinMode(PIN_BUZZER, OUTPUT);
  pinMode(PIN_LED_RED, OUTPUT);
  pinMode(PIN_LED_GREEN, OUTPUT);

  // Initial State: Gate Open
  gateServo.attach(PIN_SERVO);
  openGate();
  
  Serial.println("System Initialized: Gate Open");
}

void loop() {
  // Read Sensors (Assuming Active LOW for standard IR modules)
  int sensor1State = digitalRead(PIN_SENSOR_ARRIVAL);
  int sensor2State = digitalRead(PIN_SENSOR_DEPARTURE);
  
  // LOGIC:
  // 1. Train Arriving (Sensor 1 triggers) AND Gate is currently Open
  //    -> Warning -> Close Gate
  if (sensor1State == LOW && !isGateClosed) {
    Serial.println("Train Detected at Arrival Sensor! Closing Gate sequence...");
    closeGateSequence();
  } 
  
  // 2. Train Departing (Sensor 2 triggers) AND Gate is currently Closed
  //    -> Open Gate
  else if (sensor2State == LOW && isGateClosed) {
    Serial.println("Train Detected at Departure Sensor! Opening Gate...");
    openGate();
  }
}

void openGate() {
  digitalWrite(PIN_BUZZER, LOW);
  digitalWrite(PIN_LED_RED, LOW);
  digitalWrite(PIN_LED_GREEN, HIGH);
  
  gateServo.write(ANGLE_OPEN);
  isGateClosed = false;
  Serial.println("Gate is OPEN");
}

void closeGateSequence() {
  // 1. Warning Phase
  digitalWrite(PIN_LED_GREEN, LOW);
  digitalWrite(PIN_LED_RED, HIGH); // Danger light on immediately
  
  // Beep buzzer for 2 seconds before closing
  Serial.println("Warning: Gate closing in 2 seconds...");
  for(int i=0; i<4; i++) {
    digitalWrite(PIN_BUZZER, HIGH);
    delay(250);
    digitalWrite(PIN_BUZZER, LOW);
    delay(250);
  }
  
  // 2. Close Gate
  digitalWrite(PIN_BUZZER, HIGH); // Continuous buzzer while closing/closed
  gateServo.write(ANGLE_CLOSED);
  isGateClosed = true;
  Serial.println("Gate is CLOSED");
  
  // Optional: Turn buzzer off after movement, or keep on while closed. 
  // Project description usually implies buzzer warning *during* operation. 
  // Let's keep it on or keep it intermittent if preferred. 
  // For this code, we'll leave it ON to indicate danger, or OFF if just a transition warning.
  // Standard crossing: Bell rings while gate moves, then stops or continues.
  // Let's turn it off to save ears in simulation, or keep it if strictly "danger".
  // User asked for "Buzzer" usage.
  delay(1000); // Wait for servo to reach position
  digitalWrite(PIN_BUZZER, LOW); // Stop buzzer after gate is fully down (optional)
}
