#include <Wire.h>
#include <RTClib.h>
#include <AccelStepper.h>

RTC_DS3231 rtc;
AccelStepper stepper(AccelStepper::DRIVER, 22, 23);

// Define LDR and hall-effect sensor pins
const int LDR_PIN = A0;
const int HALL_SENSOR_OPEN_PIN = 2;
const int HALL_SENSOR_CLOSE_PIN = 3;

// Set the threshold for LDR to detect light or dark
const int LDR_THRESHOLD = 500;

// Set the number of steps required to open or close the door
const int STEPS_PER_REV = 200;
const int STEPS_TO_OPEN = STEPS_PER_REV * 2;
const int STEPS_TO_CLOSE = STEPS_PER_REV * 3;

// Set the time for the door to complete one full cycle (in seconds)
const int CYCLE_TIME = 60;

void setup() {
  pinMode(HALL_SENSOR_OPEN_PIN, INPUT_PULLUP);
  pinMode(HALL_SENSOR_CLOSE_PIN, INPUT_PULLUP);
  pinMode(LDR_PIN, INPUT);

  // Initialize RTC
  rtc.begin();
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  // Set up the stepper motor
  stepper.setMaxSpeed(1000);
  stepper.setAcceleration(1000);
}

void loop() {
  DateTime now = rtc.now();

  // Get the light level from LDR sensor
  int ldrVal = analogRead(LDR_PIN);

  // Check if it's day time (based on the LDR sensor reading)
  bool isDay = (ldrVal >= LDR_THRESHOLD);

  // Check if it's time to open or close the door
  bool isOpenTime = (now.hour() == 6 && now.minute() == 0 && now.second() == 0); // 6:00am
  bool isCloseTime = (now.hour() == 18 && now.minute() == 0 && now.second() == 0); // 6:00pm

  // Check if the hall-effect sensors have been triggered
  bool isOpenTriggered = digitalRead(HALL_SENSOR_OPEN_PIN) == LOW;
  bool isCloseTriggered = digitalRead(HALL_SENSOR_CLOSE_PIN) == LOW;

  // Check if it's time to open the door
  if (isDay && isOpenTime && !isOpenTriggered) {
    Serial.println("Opening the door");
    stepper.move(STEPS_TO_OPEN);
    while (stepper.distanceToGo() != 0) {
      stepper.run();
      delay(1);
    }
    digitalWrite(HALL_SENSOR_OPEN_PIN, HIGH);
    delay(CYCLE_TIME * 1000); // Wait for one full cycle before closing the door
  }

  // Check if it's time to close the door
  else if (!isDay && isCloseTime && !isCloseTriggered) {
    Serial.println("Closing the door");
    stepper.move(-STEPS_TO_CLOSE);
    while (stepper.distanceToGo() != 0) {
      stepper.run();
      delay(1);
    }
    digitalWrite(HALL_SENSOR_CLOSE_PIN, HIGH);
    delay(CYCLE_TIME * 1000); // Wait for one full cycle before opening the door again
  }

  // Check for manual control using buttons or Blynk app
  // You can use digitalRead or Blynk virtual pins to control the door manually

  // Reset the hall-effect sensors
  if (isOpenTriggered || isCloseTriggered) {
    digitalWrite(HALL_SENSOR_OPEN_PIN, HIGH);
    digitalWrite(HALL_SENSOR_CLOSE_PIN, HIGH);
 
