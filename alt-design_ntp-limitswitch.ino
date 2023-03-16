//

HARWARE USED: ESP32 DEV BOARD, NEMA17 BIPOLAR STEPPER MOTOR, 2X NC LIMIT SWITCH, L293D IC, TWO BUTTONS

INSTEAD OF USING A RTC (REAL TIME CLOCK) TO TELL TIME, WE HARNESS THE ESP32'S WIRELESS NETWORK CONNECTIVITY AND TRACK THE TIME USING NTP
(NETWORK TIME PROTOCOL). THIS NOT ONLY USES ONE LESS PEICE OF HARWARE CONNECTED TO THE ESP32, BUT IS MORE ACCURATE THAN MOST IF NOT ALL RTC. 
AND MORE PRECISE THAN RELYING ON A LDR (LIGHT DEPENDANT RESISTOR). iF YOU DON'T HAVE ABILITY TO CONNECT TO A NETWORK, EITHER DUE TO HARDWARE 
CAPABILITIES, OR DISTANCE FROM ACCESS POINT, GO WITH THE OTHER DESIGN (HEYLADIES.INO). ADDITIONALLY, THIS SKETCH USES LIMIT SWITCHES INSTEAD OF 
HALL EFFECT SENSORS TO STOP THE MOTOR ONCE IT REACHES THE DESIRED THRESHOLD FOR THE DOOR BEING OPEN OR CLOSED. 

//


#include <WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <AccelStepper.h>

// WiFi settings
const char* ssid = "your_SSID";
const char* password = "your_PASSWORD";

// NTP settings
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 0;
const int daylightOffset_sec = 3600;

// Stepper motor settings
const int stepsPerRevolution = 200;
const int motorPin1 = 26;  // IN1 on the ULN2003 driver
const int motorPin2 = 27;  // IN2 on the ULN2003 driver
const int motorPin3 = 14;  // IN3 on the ULN2003 driver
const int motorPin4 = 12;  // IN4 on the ULN2003 driver
AccelStepper stepper(AccelStepper::FULL4WIRE, motorPin1, motorPin3, motorPin2, motorPin4);
const int maxSpeed = 1000;
const int acceleration = 500;

// Limit switch settings
const int limitSwitchPin1 = 33;
const int limitSwitchPin2 = 32;

// Button settings
const int buttonPin1 = 4;
const int buttonPin2 = 2;

// State variables
bool isOpening = false;
bool isClosing = false;
bool isOpen = false;
bool isClosed = false;

// WiFi and NTP clients
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, ntpServer, gmtOffset_sec, daylightOffset_sec);

void setup() {
  // Set up serial communication for debugging
  Serial.begin(9600);

  // Set up the stepper motor
  stepper.setMaxSpeed(maxSpeed);
  stepper.setAcceleration(acceleration);

  // Set up the limit switches
  pinMode(limitSwitchPin1, INPUT_PULLUP);
  pinMode(limitSwitchPin2, INPUT_PULLUP);

  // Set up the buttons
  pinMode(buttonPin1, INPUT_PULLUP);
  pinMode(buttonPin2, INPUT_PULLUP);

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi!");

  // Initialize the NTP client
  timeClient.begin();
}

void loop() {
  // Update the time from the NTP server
  timeClient.update();
  int currentHour = timeClient.getHours();

  // Check if it's sunrise or sunset
  if (currentHour == 6 && !isOpening && !isOpen) {
    Serial.println("Sunrise detected, opening door...");
    isOpening = true;
    stepper.moveTo(1000000);  // move the motor one direction
  } else if (currentHour == 18 && !isClosing && !isClosed) {
    Serial.println("Sunset detected, closing door...");
    isClosing = true;
    stepper.moveTo(-1000000);  // move the motor the other direction
  }

  // Check if the limit switches have been triggered
  if (stepper.currentPosition() >= 1000000 && isOpening) {
    Serial.println("Door fully opened!");
    stepper.stop();
    isOpening = false;
    isOpen = true;
  } else if (stepper.currentPosition() <=
