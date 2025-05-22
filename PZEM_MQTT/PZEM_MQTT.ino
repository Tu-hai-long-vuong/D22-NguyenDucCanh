#include <PZEM004Tv30.h>
#include <WiFi.h> 
#include <PubSubClient.h> 
#include <ArduinoJson.h>
#include <WiFiClientSecure.h>
#include <stdio.h>

#define PZEM_RX_PIN 18
#define PZEM_TX_PIN 17

// WiFi

// const char *ssid = "Xuong";
// const char *password = "68686868";

const char *ssid = "Nha 05 - P302";
const char *password = "66668888";

// const char *ssid = "603-A3";
// const char *password = "0904263636";

unsigned long timeUpdata = millis();

// MQTT Broker
const char *mqtt_broker = "1f6c9630668e42d9969537af3ab11396.s1.eu.hivemq.cloud"; 
const char *mqtt_username = "nguyencanh178"; 
const char *mqtt_password = "canhneee"; 
const int mqtt_port = 8883; 

WiFiClientSecure espClient; 
PubSubClient client (espClient); 

void setup_Wifi(){
  Serial.println("Connecting to ");
  Serial.print(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED){
    delay(1000);
    Serial.print("...");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void publishMessage(const char* topic, String payload, bool retained){ 
  if (client.publish(topic, payload.c_str(), true)){ 
    Serial.println("Message published ["+String(topic)+"]: " + payload); 
}

void callback (char* topic, byte* payload, unsigned int length){ 
  Serial.println ("Message arrived in topic: "); 
  Serial.println (topic); 
  Serial.println ("Message: "); 
  String message;
  for (int i = 0; i < length; i++){ 
    message += (char)payload[i];
  }
  Serial.println (message); 
}

void MQTT_Connect (void){
  // kết nối với một MQTT Broker
  client.setServer (mqtt_broker, mqtt_port); 
  client.setCallback (callback); 
  while (!client.connected()){ 
    String client_id = "esp32-client"; 
    client_id += String(WiFi.macAddress()); 
    Serial.printf ("The Client %s connects to the public mqtt broker", client_id.c_str()); 
    if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)){ 
      Serial.println ("Public emqx mqtt broker connected"); 
    }
    else { 
      Serial.print ("false with state "); 
      Serial.print (client.state()); 
      delay (2000);
    }
  } 
}

HardwareSerial pzemSerial(1);

PZEM004Tv30 pzem(pzemSerial, PZEM_RX_PIN, PZEM_TX_PIN);

void setup(){
  Serial.begin(115200);

  pzemSerial.begin(9600, SERIAL_8N1, PZEM_RX_PIN, PZEM_TX_PIN);
  setup_Wifi();
  espClient.setInsecure();
  client.setCallback(callback);
}

void loop() {
  client.loop();
  MQTT_Connect();

  if (millis() - timeUpdata > 100){
    float voltage = pzem.voltage();
    float current = pzem.current();
    float power = pzem.power();
    float energy = pzem.energy();
    float frequency = pzem.frequency();
    float pf = pzem.pf();

    if (isnan(voltage)){
      Serial.println("Error reading voltage");
    }
    else if (isnan(current)){
      Serial.println("Error reading current");
    }
    else if (isnan(power)){
      Serial.println("Error reading power");
    }
    else if (isnan(energy)){
      Serial.println("Error reading energy");
    }
    else if (isnan(frequency)){
      Serial.println("Error reading frequency");
    }
    else if (isnan(pf)){
      Serial.println("Error reading power factor");
    }
    else{
      Serial.print(voltage);
      Serial.printf(" V, ");

      Serial.print(current);
      Serial.print(" A, ");

      Serial.print(power);
      Serial.print(" W, ");

      Serial.print(energy, 3);
      Serial.print(" kWh, ");

      Serial.print(frequency, 1);
      Serial.print(" Hz\n");
    }
    Serial.println();
    delay(1000);

    DynamicJsonDocument doc (1024);
    doc["Voltage"] = voltage;
    doc["Current"] = current;
    doc["Power"] = power;
    doc["Energy"] = energy;
    doc["Frequency"] = frequency;

    char mqtt_message [128]; 
    serializeJson (doc, mqtt_message); 
    publishMessage("ESP32_PZEM004", mqtt_message, true);

    timeUpdata = millis();
  }
}
