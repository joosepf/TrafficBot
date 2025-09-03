#include <Servo.h>  // Library for controlling servo motors

// Color sensor pin configuration
const int s0 = 9;  // Controls frequency scaling for color sensor
const int s1 = 10; // Controls frequency scaling for color sensor
const int s2 = 11; // Color selection pin for color sensor
const int s3 = 12; // Color selection pin for color sensor
const int sensorData = A3; // Reads color sensor data

// Global variables for storing color sensor data
int rdata = 0; // Stores red color data
int gdata = 0; // Stores green color data
int bdata = 0; // Stores blue color data

// LED pins
#define RIGHT_LED_PIN 7 // Right LED control pin
#define LEFT_LED_PIN 8  // Left LED control pin

// QTI sensor pins for line-following
#define LEFT_QTI_PIN A0   // Left QTI sensor pin
#define MIDDLE_QTI_PIN A1 // Middle QTI sensor pin
#define RIGHT_QTI_PIN A2  // Right QTI sensor pin

// Servo motor control pins
#define RIGHT_SERVO_PIN 5 // Right servo motor control pin
#define LEFT_SERVO_PIN 6  // Left servo motor control pin

// Servo motor pulse width settings
#define SERVO_MIN_PULSE 1300     // Minimum pulse width for servo motors
#define SERVO_MAX_PULSE 1700     // Maximum pulse width for servo motors
#define SERVO_STANDSTILL 1500    // Pulse width for stationary position

// QTI sensor threshold
#define QTI_THRESHOLD 75 // Threshold to detect black line

// Ultrasonic sensor pins
#define SONIC_ECHO_PIN 3 // Echo pin for ultrasonic sensor
#define SONIC_TRIG_PIN 4 // Trigger pin for ultrasonic sensor

// Button pin for toggling robot power
#define BUTTON 2                   // Button control pin
unsigned long const debounceDelay = 50; // Debounce delay for button

// Servo motor objects
Servo leftWheel;  // Left wheel servo motor
Servo rightWheel; // Right wheel servo motor

// Robot state: true = active, false = inactive
bool robotState = 0; // Robot power state
int color = 0;


void setup() {
  Serial.begin(9600); // Start serial communication

  // Color sensor pin configuration
  pinMode(s0, OUTPUT);
  pinMode(s1, OUTPUT);
  pinMode(s2, OUTPUT);
  pinMode(s3, OUTPUT);
  pinMode(sensorData, INPUT);

  // Frequency scaling configuration for color sensor
  digitalWrite(s0, HIGH);
  digitalWrite(s1, HIGH);

  // Button and LED configuration
  pinMode(BUTTON, INPUT);           // Set button as input
  pinMode(LEFT_LED_PIN, OUTPUT);    // Set left LED as output
  pinMode(RIGHT_LED_PIN, OUTPUT);   // Set right LED as output

  // Ultrasonic sensor configuration
  pinMode(SONIC_TRIG_PIN, OUTPUT);  // Set trigger pin as output
  pinMode(SONIC_ECHO_PIN, INPUT);   // Set echo pin as input

  // Attach servo motors to their pins
  leftWheel.attach(LEFT_SERVO_PIN, SERVO_MIN_PULSE, SERVO_MAX_PULSE);
  rightWheel.attach(RIGHT_SERVO_PIN, SERVO_MIN_PULSE, SERVO_MAX_PULSE);

  delay(10); // Allow time for servo initialization

  // Set wheels to stationary position
  SetWheels(SERVO_STANDSTILL, SERVO_STANDSTILL);

  // Attach interrupt to button for toggling robot power
  attachInterrupt(digitalPinToInterrupt(BUTTON), RobotPower, FALLING);
}


void loop() {
  //Measure color
  color = WhatColor();
  // If robot is active and no obstacle in front and color isn't red, follow the line
  if (robotState && NothingInFront() && color != 1) {
    OnLine(); // Execute line-following function
  }
  //If robot sees red, stops
  else if(color == 1){
    Stop();
    SetLed(HIGH, LOW);
    //delay(1000);
  } 
  //If something wrong stop
  else {
    Stop();
    SetLed(LOW, HIGH); // Stop if not active or obstacle detected
  }
}


/**
 * Description:    Detects whether there is an obstacle in front of the robot
 *                 using an ultrasonic sensor. Returns true if no obstacle 
 *                 is within 10 cm or no valid signal is received.
 * Parameters:     -
 * Return:         bool - True if the path is clear, false otherwise.
 */
bool NothingInFront() {
  // Send ultrasonic pulse to detect obstacles
  digitalWrite(SONIC_TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(SONIC_TRIG_PIN, LOW);

  // Measure time for echo to return
  uint16_t echoTime = pulseIn(SONIC_ECHO_PIN, HIGH, 1250);
  float distance = echoTime / 58.0; // Convert time to distance
  //Serial.println(distance);
  return (distance > 10.0 || distance == 0.0); // Return true if no obstacle within 10 cm
}


/**
 * Description:    Determines the predominant color detected by the TCS3200 sensor.
 *                 Distinguishes between red and other colors based on predefined
 *                 thresholds for RGB data.
 * Parameters:     -
 * Return:         int - 1 if red is detected, 0 for other colors.
 */
int WhatColor() {
  // Get red color data
  digitalWrite(s2, LOW);
  digitalWrite(s3, LOW);
  rdata = pulseIn(sensorData, LOW, 100);
  delay(20);

  // Get green color data
  digitalWrite(s2, HIGH);
  digitalWrite(s3, HIGH);
  gdata = pulseIn(sensorData, LOW, 100);
  delay(20);

  // Get blue color data
  digitalWrite(s2, LOW);
  digitalWrite(s3, HIGH);
  bdata = pulseIn(sensorData, LOW, 100);

  //Print all gotten data(HIGH- print | LOW- don't print)
  if(LOW){
  Serial.print("Red:");
  Serial.println(rdata);
  Serial.print("Green:");
  Serial.println(gdata);
  Serial.print("Blue:");
  Serial.println(bdata);
  }

  // Check if color detected meets specific criteria (example: red)
  if(rdata == 0 && gdata == 0 && bdata == 0){
    return 0;//Indicate other color
  }
  else if (rdata < 16 && (gdata > 28 || gdata < 2)  && (bdata > 28 || bdata < 2)) {
    return 1; // Indicate red color
  }
  else {
    return 0; // Indicate other color
  }
}


/**
 * Description:    Toggles the robot's power state between active and inactive
 *                 when the button is pressed.
 * Parameters:     -
 * Return:         -
 */
void RobotPower() {
  static uint32_t lastDebounceTime = 0;

  // Debounce to prevent multiple triggers from button press
  if ((millis() - lastDebounceTime) > debounceDelay) {
    lastDebounceTime = millis();
    robotState = !robotState; // Toggle robot state
  }
}

/**
 * Description:    Stops the robot by setting the wheels to a standstill
 *                 and turning off both LEDs.
 * Parameters:     -
 * Return:         -
 */
void Stop() {
  SetWheels(SERVO_STANDSTILL, SERVO_STANDSTILL); // Stop wheels
  SetLed(LOW, LOW); // Turn off LEDs
  delay(100); // Small delay for stopping action
}


/**
 * Description:    Controls the robot's movement while checking the black line.
 * Parameters:     -
 * Return:         -
 */
void OnLine() {
  // Read QTI sensors to determine position on line
  bool leftQtiSeesBlack = IsQtiDetectingBlack(LEFT_QTI_PIN);
  bool rightQtiSeesBlack = IsQtiDetectingBlack(RIGHT_QTI_PIN);
  bool middleQtiSeesBlack = IsQtiDetectingBlack(MIDDLE_QTI_PIN);

  // Movement logic based on sensor data
  if (!leftQtiSeesBlack && !rightQtiSeesBlack) {
    MoveForward(0); // Move forward if on track
  } 
  else if (middleQtiSeesBlack && rightQtiSeesBlack && leftQtiSeesBlack) {
    MoveForward(0); // Adjust right if fully on line
  } 
  else if (leftQtiSeesBlack) {
    MoveLeft(10); // Move left if left sensor detects line
  } 
  else if (rightQtiSeesBlack) {
    MoveRight(10); // Move right if right sensor detects line
  }

}


/**
 * Description:    Moves the robot slightly to the left by adjusting the pulse
 *                 width of the servo motors. Activates the left LED during the movement.
 * Parameters:     time - Duration of the movement in milliseconds.
 * Return:         -
 */
void MoveLeft(uint16_t time) {
  SetWheels(1500, 1400); // Adjust wheels for left movement
  SetLed(LOW, HIGH);     // Light up left LED
  delay(time);
}


/**
 * Description:    Moves the robot slightly to the right by adjusting the pulse
 *                 width of the servo motors. Activates the right LED during the movement.
 * Parameters:     time - Duration of the movement in milliseconds.
 * Return:         -
 */
void MoveRight(uint16_t time) {
  SetWheels(1600, 1500); // Adjust wheels for right movement
  SetLed(HIGH, LOW);      // Light up right LED
  delay(time);
}


/**
 * Description:    Moves the robot forward by adjusting the pulse width of
 *                 both servo motors equally. Activates both LEDs during the movement.
 * Parameters:     time - Duration of the movement in milliseconds.
 * Return:         -
 */
void MoveForward(uint16_t time) {
  SetWheels(1550, 1450); // Adjust wheels for forward movement
  SetLed(HIGH, HIGH);     // Light up both LEDs
  delay(time);
}


/**
 * Description:    Sets the pulse width of the left and right servo motors to control
 *                 their movement. Used for directional control and stopping the robot.
 * Parameters:     leftWheelImpulseLength  - Pulse width for the left servo.
 *                 rightWheelImpulseLength - Pulse width for the right servo.
 * Return:         -
 */
void SetWheels(int leftWheelImpulseLength, int rightWheelImpulseLength) {
  leftWheel.writeMicroseconds(leftWheelImpulseLength); // Control left wheel
  rightWheel.writeMicroseconds(rightWheelImpulseLength); // Control right wheel
}

/**
 * Description:    Checks if a specific QTI sensor detects a black line by measuring
 *                 the discharge time of its capacitor.
 * Parameters:     qtiPin - The pin number connected to the QTI sensor.
 * Return:         bool - True if a black line is detected, false otherwise.
 */
bool IsQtiDetectingBlack(uint8_t qtiPin) {
  digitalWrite(qtiPin, HIGH); // Charge QTI capacitor
  delayMicroseconds(1000);    // Wait for charge
  digitalWrite(qtiPin, LOW);  // Start discharge

  uint16_t qtiValue = analogRead(qtiPin); // Read discharge level
  Serial.println(qtiValue);
  return qtiValue > QTI_THRESHOLD;        // True if black detected
}

/**
 * Description:    Controls the state of the left and right LEDs.
 * Parameters:     leftLedState  - HIGH or LOW for the left LED.
 *                 rightLedState - HIGH or LOW for the right LED.
 * Return:         -
 */
void SetLed(uint8_t leftLedState, uint8_t rightLedState) {
  digitalWrite(RIGHT_LED_PIN, leftLedState); // Control right LED
  digitalWrite(LEFT_LED_PIN, rightLedState); // Control left LED
}

/**
 * Description:    Reads the state of the button with software debouncing.
 *                 Updates the button state only after the debounce delay has passed.
 * Parameters:     -
 * Return:         bool - Current state of the button (HIGH or LOW).
 */
bool ButtonRead() {
  static bool lastButtonState = LOW;
  static uint32_t lastDebounceTime = 0;
  static bool buttonState = LOW;

  bool btnReadValue = digitalRead(BUTTON); // Read button state
  if (btnReadValue != lastButtonState) {
    lastDebounceTime = millis(); // Update debounce timer
  }
}

