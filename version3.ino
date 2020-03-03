#include <BLEDevice.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

#include <WiFi.h>
#include <PubSubClient.h>

#define FIREBASE_HOST "indoor-localization-test.firebaseio.com"
#define FIREBASE_AUTH "KdUa2LcwLEpWFzinlmOjhZ2ZWCtQhfKPGqD8okJn"
#define WIFI_SSID "Shenali"
#define WIFI_PASSWORD "zend0325"
const char* mqtt_server = "mqtt.eclipse.org";

int device_count = 0;

BLEScan* pBLEScan;

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
    }
};


void setup() {
  Serial.begin(115200);
}

void readData(){

  Serial.println("Scanning for iBeacons...");
  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true);
//  pBLEScan->setInterval(100);
//  pBLEScan->setWindow(99);
  
  BLEScanResults foundDevices = pBLEScan->start(3, false);

  device_count = foundDevices.getCount();
  Serial.printf("Device Count: %d\n", device_count);
  
  for (uint32_t i = 0; i < device_count; i++)
  {
    
    BLEAdvertisedDevice device = foundDevices.getDevice(i);
    
    int RSSI_data = (int)device.getRSSI();
    String MAC_address = (String)device.getAddress().toString().c_str();

    Serial.printf("RSSI =  %d --- ", RSSI_data);
    Serial.printf("MAC Address =  %s", MAC_address);
    
    delay(500);
    
    }
  
  }

void connectWiFi(){

  Serial.print("Connecting to Wi-Fi");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED)
  {
      Serial.print(".");
      delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
 
  }

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("SAN", "hello Eddi");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(3000);
    }
  }
}

  
void loop() {

  connectWiFi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  readData();
  
  Serial.println("iBeacon Scan done!");
  pBLEScan->clearResults();   // delete results fromBLEScan buffer to release memory
  Serial.println("......................................................");
  delay(100);

}
