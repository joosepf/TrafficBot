// Define ultrasonic sensor pins
#define SONIC_ECHO_PIN 12 // Echo pin for the ultrasonic sensor
#define SONIC_TRIG_PIN 11 // Trigger pin for the ultrasonic sensor

// Variables for measurement timing
float measureTime = 0; // Time for the next measurement
#define MEASUREINTERVAL 72000 // Interval between measurements (in milliseconds)
float distance = 0; // Calculated distance (in centimeters)

// Traffic light 1 LED pins
const int F1Punane = 4;
const int F1Kollane = 3;
const int F1Roheline = 2;

// Traffic light 2 LED pins
const int F2Punane = 10;
const int F2Kollane = 9;
const int F2Roheline = 8;

void setup() {
  pinMode(SONIC_TRIG_PIN, OUTPUT); // Set ultrasonic trigger pin as output
  pinMode(SONIC_ECHO_PIN, INPUT); // Set ultrasonic echo pin as input

  // Set traffic light 1 pins as output
  pinMode(F1Punane, OUTPUT);
  pinMode(F1Kollane, OUTPUT);
  pinMode(F1Roheline, OUTPUT);

  // Set traffic light 2 pins as output
  pinMode(F2Punane, OUTPUT);
  pinMode(F2Kollane, OUTPUT);
  pinMode(F2Roheline, OUTPUT);

  // Initial state: Traffic light 1 red, Traffic light 2 red
  digitalWrite(F1Punane, HIGH);
  digitalWrite(F2Punane, HIGH);

  Serial.begin(9600); // Initialize serial communication for debugging
}

void loop() {
  distance = GetDistInCm(); // Measure distance using the ultrasonic sensor
  Serial.println(distance); // Print the measured distance to the serial monitor

  // Check if it's time for the next measurement
  if (millis() >= measureTime) {
    if (distance < 10.0 && distance > 1.0) {
      SecFoor();
      measureTime += MEASUREINTERVAL; // Update the measurement interval
    } else {
      NormaalSeisund();
    }
  }
}

/**
 * Description:    Activates the secondary traffic light mode.
 *                 Simulates a transition from red to green for the secondary traffic light.
 * Parameters:     -
 * Return:         -
 */
void SecFoor() {
  // Transition state: yellow light on for both traffic lights
  digitalWrite(F1Punane, LOW);
  digitalWrite(F1Kollane, HIGH);
  digitalWrite(F1Roheline, LOW);

  digitalWrite(F2Punane, LOW);
  digitalWrite(F2Kollane, HIGH);
  digitalWrite(F2Roheline, LOW);

  delay(1000); // Wait for 1 second

  // Traffic light 1: red, Traffic light 2: green
  digitalWrite(F1Punane, HIGH);
  digitalWrite(F1Kollane, LOW);
  digitalWrite(F1Roheline, LOW);

  digitalWrite(F2Punane, LOW);
  digitalWrite(F2Kollane, LOW);
  digitalWrite(F2Roheline, HIGH);

  delay(10000); // Wait for 10 seconds

  // Transition state: yellow light on for both traffic lights
  digitalWrite(F1Punane, LOW);
  digitalWrite(F1Kollane, HIGH);
  digitalWrite(F1Roheline, LOW);

  digitalWrite(F2Punane, LOW);
  digitalWrite(F2Kollane, HIGH);
  digitalWrite(F2Roheline, LOW);

  delay(1000); // Wait for 1 second

  NormaalSeisund(); // Return to the normal state
}

/**
 * Description:    Resets traffic lights to their default state.
 *                 Traffic light 1: green, Traffic light 2: red.
 * Parameters:     -
 * Return:         -
 */
void NormaalSeisund() {
  digitalWrite(F1Punane, LOW);
  digitalWrite(F1Kollane, LOW);
  digitalWrite(F1Roheline, HIGH);

  digitalWrite(F2Punane, HIGH);
  digitalWrite(F2Kollane, LOW);
  digitalWrite(F2Roheline, LOW);
}

/**
 * Description:    Measures the distance using the ultrasonic sensor and returns the result in centimeters.
 * Parameters:     -
 * Return:         Distance in centimeters (float).
 */
float GetDistInCm() {
  digitalWrite(SONIC_TRIG_PIN, HIGH); // Send ultrasonic pulse
  delayMicroseconds(10); // Duration of the pulse
  digitalWrite(SONIC_TRIG_PIN, LOW);

  uint16_t echoTime = pulseIn(SONIC_ECHO_PIN, HIGH, 2400); // Measure echo time
  return echoTime / 58.0; // Convert echo time to distance in centimeters
}
