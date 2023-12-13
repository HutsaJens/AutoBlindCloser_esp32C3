#include <WiFi.h>
#include <PubSubClient.h>  // Import PubSubClient library to initialize MQTT protocol
#include "StepperDriver.h"

// Update these with values suitable for your network.
const char* ssid = "Ziggo-Bertens-2.4Ghz";  //use your ssid
const char* password = "BertensWifi2018";   //use your password
const char* mqtt_server = "192.168.178.7";

WiFiClient espClient;
PubSubClient client(espClient);

unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE (50)
char msg[MSG_BUFFER_SIZE];
int value = 0;

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    yield();
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

// Check for Message received on define topic for MQTT Broker
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");

  char buffer[length + 1];
  memcpy(buffer, payload, length);
  buffer[length] = '\0';
  Serial.println(buffer);

  yield();

  // Switch on the LED if an 1 was received as first character
  if (buffer[0] == '1') {
    digitalWrite(LED_BUILTIN, LOW);  // Turn the LED on (Led is active low)
  } else if (buffer[0] == '0') {
    digitalWrite(LED_BUILTIN, HIGH);  // Turn the LED off by making the voltage HIGH
  } else if (buffer[0] == 'M') {

    char data[3];
    snprintf(data, sizeof(data), "%c%c", buffer[1], buffer[2]);
    int stepsPercentage = atoi(data);

    // Check for "CCW" or "CW"
    if (strstr(buffer, "CCW") != nullptr) {
      Serial.println("Received CCW");
      rotate(true, stepsPercentage);
    } else if (strstr(buffer, "CW") != nullptr) {
      Serial.println("Received CW");
      rotate(false, stepsPercentage);
    } else {
      Serial.println(F("Unknown payload"));
    }
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println(F("connected"));
      // Once connected, publish an announcement...
      client.publish("LEDTEST2", "hello world");
      // ... and resubscribe
      client.subscribe("LEDTEST1");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
    yield();
  }
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);  // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  setupStepper();
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // unsigned long now = millis();
  // if (now - lastMsg > 2000) {
  //   lastMsg = now;
  //   ++value;
  //   snprintf (msg, MSG_BUFFER_SIZE, "hello world #%ld", value);
  //   Serial.print("Publish message: ");
  //   Serial.println(msg);
  //   client.publish("LEDTEST2", msg);
  // }
}
