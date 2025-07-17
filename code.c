#include <LiquidCrystal.h> // LCD library if needed later

// Ultrasonic sensor pins
const int trigPin1 = 10;
const int echoPin1 = 11;
const int trigPin2 = 13;
const int echoPin2 = 8;
const int trigPin3 = 6;
const int echoPin3 = 7;

// Other input/output components
const int potPin = A0;            // Potentiometer to detect turn direction
const int redPin = 5;             // RED LED for danger alert
const int greenPin = 6;           // GREEN LED for safe alert
const int buzzerPin = 9;          // Buzzer for warning
const int indicatorLeftPin = 4;   // Left indicator switch input
const int indicatorRightPin = 3;  // Right indicator switch input

// Distance and direction variables
long duration1, duration2, duration3;
int distance1, distance2, distance3;
int potValue;
String direction = "None";

void setup() {
  // Set up sensor and IO pin modes
  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT);
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);
  pinMode(trigPin3, OUTPUT);
  pinMode(echoPin3, INPUT);
  
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(indicatorLeftPin, INPUT);
  pinMode(indicatorRightPin, INPUT);

  Serial.begin(9600); // Begin serial communication for debugging
}

// Function to get distance from ultrasonic sensor
int getDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  return pulseIn(echoPin, HIGH) * 0.034 / 2;
}

void loop() {
  // Read distances from all three ultrasonic sensors
  distance1 = getDistance(trigPin1, echoPin1);
  distance2 = getDistance(trigPin2, echoPin2);
  distance3 = getDistance(trigPin3, echoPin3);
  
  // Read turn angle from potentiometer
  potValue = analogRead(potPin);
  
  Serial.print("D1: "); Serial.print(distance1);
  Serial.print(" | D2: "); Serial.print(distance2);
  Serial.print(" | D3: "); Serial.print(distance3);
  Serial.print(" | Pot: "); Serial.println(potValue);

  // Determine vehicle direction based on ultrasonic sensor readings
  if (distance1 > 2 && distance3 > 2 && distance2 > 2) {
    direction = "Parallel";
  } else if (distance1 > 2 && distance3 > 2) {
    direction = "Left";
  } else if (distance1 > 2) {
    direction = "Left";
  } else if (distance2 > 2 && distance3 > 2) {
    direction = "Right";
  } else if (distance2 > 2) {
    direction = "Right";
  } else {
    direction = "None";
  }

  Serial.println("Direction: " + direction);

  // Determine the intended turn direction using potentiometer
  String turnDirection = (potValue > 500) ? "Left" : "Right";
  Serial.println("Turn Direction: " + turnDirection);

  // Read the status of indicator switches
  bool leftIndicatorOn = digitalRead(indicatorLeftPin);
  bool rightIndicatorOn = digitalRead(indicatorRightPin);

  // Check if vehicle is coming in same direction as turn
  if (direction == turnDirection) {
    Serial.println("ALERT! Potential collision risk.");

    // If indicator is ON, it's safe
    if ((turnDirection == "Left" && leftIndicatorOn) || (turnDirection == "Right" && rightIndicatorOn)) {
      Serial.println("Indicator ON: Safe Turn");
      digitalWrite(redPin, LOW);
      digitalWrite(greenPin, HIGH);
      digitalWrite(buzzerPin, LOW);
    } else {
      // Collision risk - indicator is OFF
      Serial.println("Indicator OFF: Collision Warning!");
      digitalWrite(redPin, HIGH);
      digitalWrite(greenPin, LOW);
      digitalWrite(buzzerPin, HIGH);

      delay(10000); // Wait for driver to react (e.g., turn on indicator)

      // Re-check indicator status
      leftIndicatorOn = digitalRead(indicatorLeftPin);
      rightIndicatorOn = digitalRead(indicatorRightPin);

      if ((turnDirection == "Left" && leftIndicatorOn) || (turnDirection == "Right" && rightIndicatorOn)) {
        Serial.println("Indicator Turned ON After Delay: Safe Turn Now");
        digitalWrite(redPin, LOW);
        digitalWrite(greenPin, HIGH);
        digitalWrite(buzzerPin, LOW);
      } else {
        Serial.println("Indicator Still OFF: Danger");
        digitalWrite(redPin, HIGH);
        digitalWrite(greenPin, LOW);
        digitalWrite(buzzerPin, HIGH);
      }
    }
  }

  delay(11000); // Wait before the next loop iteration
}
