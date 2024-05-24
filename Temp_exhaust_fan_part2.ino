#include <DHT.h>

#define DHTPIN 7      // Pin connected to the DHT11 sensor
#define DHTTYPE DHT11 // DHT type is DHT11

DHT dht(DHTPIN, DHTTYPE);
int motor_pin = 5;

void setup() {
  pinMode(motor_pin, OUTPUT);
  Serial.begin(9600);
  dht.begin(); // Initialize DHT sensor
}

void loop() {
  delay(2000); // Wait for 2 seconds between readings

  float temperature = dht.readTemperature(); // Read temperature in Celsius
  float humidity = dht.readHumidity();       // Read humidity

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  Serial.print("Temperature = ");
  Serial.print(temperature);
  Serial.println(" °C");
  Serial.print("Humidity = ");
  Serial.print(humidity);
  Serial.println(" %");

  if ( dht.readTemperature() >= 34.30) {
    Serial.println("Temperature is above threshold....turning on the motor");
    digitalWrite(motor_pin, HIGH);
    delay(5000);
    digitalWrite(motor_pin, LOW);

  } 
  else if( dht.readTemperature() < 34.30) {
    Serial.println("Temperature is below the threshold values....turning off the motor");
    digitalWrite(motor_pin, LOW);
    
  }
  else{
        digitalWrite(motor_pin, LOW);

  }
}
