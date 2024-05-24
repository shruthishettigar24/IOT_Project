#define BLYNK_TEMPLATE_ID "TMPL3qA-6jg1F"
#define BLYNK_TEMPLATE_NAME "my project"
#define BLYNK_AUTH_TOKEN "rTm2emia_WFdNC3bBWPb1LUC8BqbtNDr"
#define BLYNK_PRINT Serial

#include <Arduino.h>
#include <HX711.h>
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <ESP_Mail_Client.h>

// Load cell pins
uint8_t dataPin = 19; // for ESP32
uint8_t clockPin = 18; // for ESP32

// Buzzer pin
int buzzerPin = 6;

// WiFi credentials
char ssid[] = "Shruthi";
char pass[] = "11112222";

// Blynk authentication token
char auth[] = BLYNK_AUTH_TOKEN;

// Email credentials
#define SMTP_HOST "smtp.gmail.com"
#define SMTP_PORT 465
#define AUTHOR_EMAIL "shruthishettigar917@gmail.com"
#define AUTHOR_PASSWORD "yeuy haos ixfv uqgi" // Use app-specific password if 2FA is enabled
#define RECIPIENT_NAME "Kavya"
#define RECIPIENT_EMAIL "kavyasutaria30@gmail.com"

// Global variables
HX711 scale;
SMTPSession smtp;
Session_Config config; // Move Session_Config to global scope
int weight = 0;
bool emailSent = false; // Add a flag to track if the email has been sent

// Function prototypes
void setup();
void loop();
void sendLowGasEmail();

BLYNK_WRITE(V0) {
  int read = param.asInt();
  if (read == 1) {
    weight = scale.get_units(10);
    Serial.print("UNITS: ");
    Serial.println(weight);
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println(__FILE__);
  Serial.print("LIBRARY VERSION: ");
  Serial.println(HX711_LIB_VERSION);
  Serial.println();

  scale.begin(dataPin, clockPin);
  scale.tare(); // Reset the scale to 0

  Serial.print("UNITS: ");
  Serial.println(scale.get_units(10));

  Serial.println("\nEmpty the scale, press a key to continue");
  while (!Serial.available());
  while (Serial.available()) Serial.read();

  scale.tare();
  Serial.print("UNITS: ");
  Serial.println(scale.get_units(10));

  Serial.println("\nPut 1000 gram on the scale, press a key to continue");
  while (!Serial.available());
  while (Serial.available()) Serial.read();

  scale.calibrate_scale(1000, 5); // Calibrate with known weight
  Serial.print("UNITS: ");
  Serial.println(scale.get_units(10));

  Serial.println("\nScale is calibrated, press a key to continue");
  while (!Serial.available());
  while (Serial.available()) Serial.read();

  // Initialize buzzer pin
  pinMode(buzzerPin, OUTPUT);

  // Connect to WiFi
  WiFi.begin(ssid, pass);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nConnected to Wi-Fi");

  // Connect to Blynk
  Blynk.begin(auth, ssid, pass);
  Serial.println("Connected to Blynk");

  // Initialize SMTP session
  MailClient.networkReconnect(true);
  smtp.debug(1); // Enable debugging

  // Configure the SMTP session
  config.server.host_name = SMTP_HOST;
  config.server.port = SMTP_PORT;
  config.login.email = AUTHOR_EMAIL;
  config.login.password = AUTHOR_PASSWORD;
  config.login.user_domain = "";

  if (!smtp.connect(&config)) {
    Serial.println("Failed to connect to SMTP server");
    Serial.println(smtp.errorReason());
  } else {
    Serial.println("Connected to SMTP server");
  }
}

void loop() {
  if (!emailSent) { // Only check gas level if email hasn't been sent
    // Read gas level
    float gasLevel = scale.get_units(10);
    Serial.print("Gas level: ");
    Serial.println(gasLevel);

    // Display gas level on Blynk gauge widget
    Blynk.virtualWrite(V1, gasLevel);

    // Check gas level threshold
    if (gasLevel < 700) { // Change threshold to 700
      // Send email notification
      sendLowGasEmail();

      // Trigger buzzer for 5 seconds
      for (int i = 0; i < 5; i++) {
        digitalWrite(buzzerPin, HIGH);
        delay(500);
        digitalWrite(buzzerPin, LOW);
        delay(500);
      }
    }
  }

  // Ensure Blynk connection
  Blynk.run();

  delay(1000); // Add delay to avoid spamming email server
}

void sendLowGasEmail() {
  SMTP_Message message;
  message.sender.name = F("ESP");
  message.sender.email = AUTHOR_EMAIL;
  message.subject = F("Low Gas Level Alert!");
  message.addRecipient(RECIPIENT_NAME, RECIPIENT_EMAIL);

  String textMsg = "Gas level is low! Please book a gas cylinder.";
  message.text.content = textMsg.c_str();
  message.text.charSet = "us-ascii";
  message.text.transfer_encoding = Content_Transfer_Encoding::enc_7bit;

  message.priority = esp_mail_smtp_priority::esp_mail_smtp_priority_low;
  message.response.notify = esp_mail_smtp_notify_success | esp_mail_smtp_notify_failure | esp_mail_smtp_notify_delay;

  if (!MailClient.sendMail(&smtp, &message)) {
    Serial.println("Failed to send email");
    Serial.println(smtp.errorReason()); // Print detailed error message
  } else {
    Serial.println("Low gas email sent");
    emailSent = true; // Set the flag to true to indicate the email has been sent
  }
}





