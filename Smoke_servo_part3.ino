#include <Servo.h>

Servo myservo;  // Create servo object to control a servo

int pos = 0;              // Variable to store the servo position
bool gas_leakage_detected = false;  // Variable to check if gas leakage is detected

int sensor_pin = A1;      // Pin connected to the smoke sensor
//int buzzer = 7;           // Pin connected to the buzzer
int threshold = 222;      // Threshold value for gas detection

void setup() {
  myservo.attach(9);      // Attach the servo to digital pin 9
  Serial.begin(9600);     // Start serial communication

  pinMode(sensor_pin, INPUT);
 // pinMode(buzzer, OUTPUT);

  Serial.println("Warming up the MQ2 sensor");
  delay(2000); // Warm-up time for the sensor
}

void loop() {
  int gasState = analogRead(sensor_pin); // Read the gas sensor value
  Serial.print("Gas State: ");
  Serial.println(gasState);

  if (gasState < threshold) {
    Serial.println("NO gas leakage is present");
    //digitalWrite(buzzer, LOW);

    if (gas_leakage_detected) {
      Serial.println("Sweeping back from 180 to 0 degrees");
      // If gas was previously detected but now it's not, sweep back to 0 degrees
      for (pos = 180; pos >= 0; pos -= 1) { // Sweep from 180 to 0 degrees
        myservo.write(pos);                // Tell servo to go to position in variable 'pos'
        delay(15);                         // Wait 15 ms for the servo to reach the position
        //Serial.print("Servo position: ");
        //Serial.println(pos);
      }
      gas_leakage_detected = false;
    }
  } else {
    Serial.println("Alert!!! Gas leakage");
    //digitalWrite(buzzer, HIGH);

    if (!gas_leakage_detected) { // Check if the sweep is not done
      Serial.println("Sweeping from 0 to 180 degrees");
      for (pos = 0; pos <= 180; pos += 1) { // Sweep from 0 to 180 degrees
        myservo.write(pos);                // Tell servo to go to position in variable 'pos'
        delay(15);                         // Wait 15 ms for the servo to reach the position
        //Serial.print("Servo position: ");
        //Serial.println(pos);
      }
      gas_leakage_detected = true; // Set the gas_leakage_detected flag to true to indicate the sweep is complete
    }
  }

  delay(2000); // Wait for 2 seconds before the next reading
}
