#include <WiFi.h>
#include <HTTPClient.h>
#include <PubSubClient.h>

float temp;

#ifndef STASSID
#define STASSID "My Zone" 
#define STAPSK  "sreevani" 
#endif
const char* ssid     = STASSID;
const char* password = STAPSK;

char mqttUserName[] = "HiUuMi4XOBUaICcJGwgoOwc";        
char mqttPass[] = "LdCkUR/SR7W1prie6u5V17WG";        
char writeAPIKey[] = "ZXWA4EB1VTN19CAU";    
long channelID = 1453368;                   
int fieldNumber = 1;                         

static const char alphanum[] ="0123456789"
                              "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                              "abcdefghijklmnopqrstuvwxyz";  // For random generation of client ID.

WiFiClient client;                           // Initialize the Wi-Fi client library.

PubSubClient mqttClient(client);    // Initialize the PubSubClient library.
const char* server = "mqtt.thingspeak.com"; 

unsigned long lastConnectionTime = 0; 
const unsigned long postingInterval = 25L * 1000L; // Post data every 25 seconds.



void setup() {
  Serial.begin(115200);
  
  WiFi.begin(ssid, password); 
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Waiting to connect...");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
  mqttClient.setServer(server, 1883);   // Set the MQTT broker details.

  
  delay(1000);
  
}


void loop() {

 // Reconnect if MQTT client is not connected.
  if (!mqttClient.connected()) 
  {
    reconnect();
  }

  mqttClient.loop();   // Call the loop continuously to establish connection to the server.

  // If interval time has passed since the last connection, publish data to ThingSpeak.
  if (millis() - lastConnectionTime > postingInterval) 
  {
  
    mqttPublishFeed(); 
    Serial.println(temp);
  }
}

void reconnect() {
  char clientID[9];

  // Loop until reconnected.
  while (!mqttClient.connected()) 
  {
    Serial.print("Attempting MQTT connection...");
    // Generate ClientID
    for (int i = 0; i < 8; i++) {
        clientID[i] = alphanum[random(51)];
    }
    clientID[8]='\0';
    Serial.println("clientID is ");
    Serial.print(clientID);
    // Connect to the MQTT broker.
    if (mqttClient.connect(clientID,mqttUserName,mqttPass)) 
    {
      Serial.println("connected");
    } else 
    {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void mqttPublishFeed() {
  
  temp = random(300); // Read temperature from DHT sensor.
  // Create data string to send to ThingSpeak.
  String data = "field1=" +  String(temp, DEC);
  int length = data.length();
  const char *msgBuffer;
  msgBuffer=data.c_str();
  Serial.println(msgBuffer);
  
  // Create a topic string and publish data to ThingSpeak channel feed. 
  String topicString = "channels/" + String( channelID ) + "/publish/"+String(writeAPIKey);
  length = topicString.length();
  const char *topicBuffer;
  topicBuffer = topicString.c_str();
  mqttClient.publish( topicBuffer, msgBuffer );
  lastConnectionTime = millis();
}
