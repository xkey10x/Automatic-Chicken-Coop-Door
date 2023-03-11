// Pin assignments
int pirPin = 2; // PIR sensor pin
int buzzerPin = 3; // Buzzer pin
int ledPin = 4; // LED pin
int motorPin = 5; // DC motor pin

// Variables
int pirState = LOW; // current state of the PIR sensor
int lastPirState = LOW; // previous state of the PIR sensor
unsigned long lastTriggerTime = 0; // last time the PIR sensor was triggered
int buzzerDuration = 1000; // duration of the buzzer sound in milliseconds
int ledDuration = 500; // duration of the flashing LED in milliseconds
int motorDuration = 5000; // duration of the spinning DC motor in milliseconds

void setup() {
  pinMode(pirPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(motorPin, OUTPUT);
}

void loop() {
  // Read PIR sensor state
  pirState = digitalRead(pirPin);

  // Check if PIR sensor has been triggered
  if (pirState == HIGH && lastPirState == LOW && millis() - lastTriggerTime > 5000) {
    // Save trigger time
    lastTriggerTime = millis();

    // Activate alarm
    activateAlarm();
  }

  // Update PIR sensor state
  lastPirState = pirState;
}

void activateAlarm() {
  // Turn on buzzer and flashing LED
  digitalWrite(buzzerPin, HIGH);
  digitalWrite(ledPin, HIGH);

  // Spin DC motor
  digitalWrite(motorPin, HIGH);
  delay(motorDuration);
  digitalWrite(motorPin, LOW);

  // Turn off buzzer and flashing LED
  digitalWrite(buzzerPin, LOW);
  digitalWrite(ledPin, LOW);
}
