#define WIFI_SSID "WifiIoT"
#define WIFI_PASS "12345678"

#define BOT_TOKEN "8159224277:AAH7vIE2rGfqTHcCIixaSyrkOpVa7Fqmnjs"
#define CHAT_ID "7035391533"

// MQTT Configuration
const char* mqtt_server = "a21d026aee9c471395ac1ab161eb8f48.s1.eu.hivemq.cloud";
const int   mqtt_port   = 8883;
const char* mqtt_user   = "wololo";    
const char* mqtt_pass   = "Wololou6";  
const char* mqtt_topic_status  = "perta/status";
const char* mqtt_topic_motion  = "perta/motion";
const char* mqtt_topic_notify  = "perta/notify";

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <FastBot.h>     // library telegram
#include <PubSubClient.h> // library MQTT
#include <ArduinoJson.h>  // library untuk JSON

FastBot bot(BOT_TOKEN);

// MQTT Client Setup
WiFiClientSecure espClient;
PubSubClient mqttClient(espClient);

// Reduce MQTT buffer size to save memory
#define MQTT_MAX_PACKET_SIZE 512

#include "camera.h"
camera_fb_t* frame;

#define pin_led_flash 4
#define pin_led_red 15
#define pin_rcwl 13

unsigned long last_millis1 = 0, last_millis2 = 0;
unsigned long last_mqtt_publish = 0;

bool ruang_tidak_dipakai = 0;
bool ruang_sedang_dipakai = 1;
bool status_ruangan = ruang_tidak_dipakai;

bool motion_detected = false;
bool flag_status_change = false;

// Root CA untuk HiveMQ Cloud
const char* root_ca = R"(
-----BEGIN CERTIFICATE-----
MIIFazCCA1OgAwIBAgIRAIIQz7DSQONZRGPgu2OCiwAwDQYJKoZIhvcNAQELBQAw
TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh
cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMTUwNjA0MTEwNDM4
WhcNMzUwNjA0MTEwNDM4WjBPMQswCQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJu
ZXQgU2VjdXJpdHkgUmVzZWFyY2ggR3JvdXAxFTATBgNVBAMTDElTUkcgUm9vdCBY
MTCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBAK3oJHP0FDfzm54rVygc
h77ct984kIxuPOZXoHj3dcKi/vVqbvYATyjb3miGbESTtrFj/RQSa78f0uoxmyF+
0TM8ukj13Xnfs7j/EvEhmkvBioZxaUpmZmyPfjxwv60pIgbz5MDmgK7iS4+3mX6U
A5/TR5d8mUgjU+g4rk8Kb4Mu0UlXjIB0ttov0DiNewNwIRt18jA8+o+u3dpjq+sW
T8KOEUt+zwvo/7V3LvSye0rgTBIlDHCNAymg4VMk7BPZ7hm/ELNKjD+Jo2FR3qyH
B5T0Y3HsLuJvW5iB4YlcNHlsdu87kGJ55tukmi8mxdAQ4Q7e2RCOFvu396j3x+UC
B5iPNgiV5+I3lg02dZ77DnKxHZu8A/lJBdiB3QW0KtZB6awBdpUKD9jf1b0SHzUv
KBds0pjBqAlkd25HN7rOrFleaJ1/ctaJxQZBKT5ZPt0m9STJEadao0xAH0ahmbWn
OlFuhjuefXKnEgV4We0+UXgVCwOPjdAvBbI+e0ocS3MFEvzG6uBQE3xDk3SzynTn
jh8BCNAw1FtxNrQHusEwMFxIt4I7mKZ9YIqioymCzLq9gwQbooMDQaHWBfEbwrbw
qHyGO0aoSCqI3Haadr8faqU9GY/rOPNk3sgrDQoo//fb4hVC1CLQJ13hef4Y53CI
rU7m2Ys6xt0nUW7/vGT1M0NPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNV
HRMBAf8EBTADAQH/MB0GA1UdDgQWBBR5tFnme7bl5AFzgAiIyBpY9umbbjANBgkq
hkiG9w0BAQsFAAOCAgEAVR9YqbyyqFDQDLHYGmkgJykIrGF1XIpu+ILlaS/V9lZL
ubhzEFnTIZd+50xx+7LSYK05qAvqFyFWhfFQDlnrzuBZ6brJFe+GnY+EgPbk6ZGQ
3BebYhtF8GaV0nxvwuo77x/Py9auJ/GpsMiu/X1+mvoiBOv/2X/qkSsisRcOj/KK
NFtY2PwByVS5uCbMiogziUwthDyC3+6WVwW6LLv3xLfHTjuCvjHIInNzktHCgKQ5
ORAzI4JMPJ+GslWYHb4phowim57iaztXOoJwTdwJx4nLCgdNbOhdjsnvzqvHu7Ur
TkXWStAmzOVyyghqpZXjFaH3pO3JLF+l+/+sKAIuvtd7u+Nxe5AW0wdeRlN8NwdC
jNPElpzVmbUq4JUagEiuTDkHzsxHpFKVK7q4+63SM1N95R1NbdWhscdCb+ZAJzVc
oyi3B43njTOQ5yOf+1CceWxG1bQVs5ZufpsMljq4Ui0/1lvh+wjChP4kqKOJ2qxq
4RgqsahDYVvTH9w7jXbyLeiNdd8XM2w9U/t7y0Ff/9yi0GE44Za4rF2LN9d11TPA
mRGunUHBcnWEvgJBQl9nJEiU0Zsnvgc/ubhPgXRR4Xq37Z0j4r7g1SgEEzwxA57d
emyPxgcYxn/eR44/KJ4EBs+lVDR3veyJm+kXQ99b21/+jh5Xos1AnX5iItreGCc=
-----END CERTIFICATE-----
)";

void setup()
{
  Serial.begin(115200);

  // Set LED Flash as output
  pinMode(pin_led_flash, OUTPUT);
  digitalWrite(pin_led_flash, LOW);     // jika HIGH LED ON, jika LOW LED OFF

  pinMode(pin_led_red, OUTPUT);
  digitalWrite(pin_led_red, HIGH);     // jika HIGH LED ON, jika LOW LED OFF

  pinMode(pin_rcwl, INPUT);

  Serial.println("Camera:");

  if (initCamera())
    Serial.println("Camera OK");
  else
    Serial.println("Camera Error");
    
  // Configure camera for lower memory usage
  sensor_t * s = esp_camera_sensor_get();
  if (s) {
    s->set_framesize(s, FRAMESIZE_VGA); // Reduce from default to VGA
    s->set_quality(s, 12); // 0-63, lower means higher quality
  }

  delay(500);

  connectWiFi();

  // Setup MQTT
  espClient.setCACert(root_ca);
  mqttClient.setServer(mqtt_server, mqtt_port);
  connectMQTT();

  bot.sendMessage("Bot Ready!", CHAT_ID);

  // Kirim status awal ke MQTT
  publishStatusToMQTT();

  delay(3000);

  last_millis1 = last_millis2 = millis();
}

int state = LOW;     // by default, no motion detected

void loop()
{
  bot.tick();
  
  // Maintain MQTT connection
  if (!mqttClient.connected()) {
    connectMQTT();
  }
  mqttClient.loop();

  // Publish status setiap 30 detik
  if (millis() - last_mqtt_publish > 30000) {
    publishStatusToMQTT();
    last_mqtt_publish = millis();
  }

  if (digitalRead(pin_rcwl) == HIGH)     // terdeteksi gerakan
  {
    digitalWrite(pin_led_red, LOW);     //LED red ON

    if (state == LOW)
    {
      Serial.println("Motion detected!");
      state = HIGH;     // update variable state to HIGH
      
      // Publish motion detected to MQTT
      publishMotionToMQTT(true);

      if (status_ruangan == ruang_tidak_dipakai)
      {
        sendPhotoTelegram();
        FB_Time t = bot.getTime(7);
        String pesan = "Perhatian. Terdeteksi gerakan pada " + t.dateString() + " " + t.timeString();
        bot.sendMessage(pesan, CHAT_ID);     // kirim pesan notifikasi
        Serial.println();
        Serial.println(pesan);
        
        // Kirim notifikasi ke MQTT
        publishNotificationToMQTT(pesan);
      }
    }

    last_millis2 = millis();

    if (status_ruangan == ruang_tidak_dipakai && flag_status_change == false)
    {
      flag_status_change = true;
      Serial.println("flag_status_change set to true");
      last_millis1 = millis();
    }

    if (flag_status_change == true && (millis() - last_millis1) / 1000 >= 120) // 2 menit
    {
      status_ruangan = ruang_sedang_dipakai;
      flag_status_change = false;
      Serial.println("status_ruangan : ruang_sedang_dipakai!");
      bot.sendMessage("status_ruangan : ruang_sedang_dipakai!", CHAT_ID);     // kirim pesan notifikasi
      
      // Update status ke MQTT
      publishStatusToMQTT();
      publishNotificationToMQTT("Ruangan sedang dipakai");
    }
  }
  else
  {
    digitalWrite(pin_led_red, HIGH);     //LED red OFF

    if (state == HIGH)
    {
      Serial.println("Motion stopped!");
      state = LOW;     // update variable state to LOW
      
      // Publish motion stopped to MQTT
      publishMotionToMQTT(false);
    }

    if (status_ruangan == ruang_sedang_dipakai && (millis() - last_millis2) / 1000 >= 300) // 5 menit
    {
      status_ruangan = ruang_tidak_dipakai;
      flag_status_change = false;
      Serial.println("status_ruangan : ruang_tidak_dipakai!");
      bot.sendMessage("status_ruangan : ruang_tidak_dipakai!", CHAT_ID);     // kirim pesan notifikasi
      
      // Update status ke MQTT
      publishStatusToMQTT();
      publishNotificationToMQTT("Ruangan tidak dipakai");
    }

    if (status_ruangan == ruang_tidak_dipakai && flag_status_change == true && (millis() - last_millis1) / 1000 >= 180) // 3 menit
    {
      flag_status_change = false;
      Serial.println("flag_status_change set to false");
    }
  }

  delay(10);
}

void connectWiFi()
{
  Serial.print("Connecting wifi to : " + String(WIFI_SSID));

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
    if (millis() > 20000) ESP.restart();
  }

  Serial.println(" OK! Success Connected!");
}

void connectMQTT() {
  while (!mqttClient.connected()) {
    Serial.print("Connecting to MQTT...");
    
    String clientId = "ESP32CAM-" + String(random(0xffff), HEX);
    
    if (mqttClient.connect(clientId.c_str(), mqtt_user, mqtt_pass)) {
      Serial.println(" connected");
    } else {
      Serial.print(" failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" retrying in 5 seconds");
      delay(5000);
    }
  }
}

void publishStatusToMQTT() {
  StaticJsonDocument<128> doc; // Reduced from 200
  doc["status"] = status_ruangan == ruang_sedang_dipakai ? "occupied" : "empty";
  doc["timestamp"] = millis();
  
  char jsonBuffer[128]; // Reduced from 256
  serializeJson(doc, jsonBuffer);
  
  if (mqttClient.publish(mqtt_topic_status, jsonBuffer)) {
    Serial.println("Status published to MQTT");
  }
}

void publishMotionToMQTT(bool detected) {
  StaticJsonDocument<128> doc; // Reduced from 200
  doc["motion"] = detected;
  doc["timestamp"] = millis();
  
  char jsonBuffer[128]; // Reduced from 256
  serializeJson(doc, jsonBuffer);
  
  if (mqttClient.publish(mqtt_topic_motion, jsonBuffer)) {
    Serial.println("Motion status published to MQTT");
  }
}

void publishNotificationToMQTT(String message) {
  StaticJsonDocument<256> doc; // Reduced from 300
  doc["message"] = message;
  doc["timestamp"] = millis();
  
  char jsonBuffer[384]; // Reduced from 512
  serializeJson(doc, jsonBuffer);
  
  if (mqttClient.publish(mqtt_topic_notify, jsonBuffer)) {
    Serial.println("Notification published to MQTT");
  }
}

void sendPhotoTelegram()
{
  // Free up memory before taking photo
  mqttClient.disconnect(); // Temporarily disconnect MQTT to free memory
  delay(100);
  
  /* ---------------------------------------- The process of taking photos. */
  Serial.println("Taking a photo...");
  Serial.print("Free heap: ");
  Serial.println(ESP.getFreeHeap());

  digitalWrite(pin_led_flash, HIGH);     //  Turns on the LED Flash  */
  delay(500);

  //Dispose first picture because of bad quality
  frame = NULL;
  frame = esp_camera_fb_get();
  if (frame) {
    esp_camera_fb_return(frame);     // dispose the buffered image
  }
  delay(100);

  //Dispose second picture because of bad quality
  frame = NULL;
  frame = esp_camera_fb_get();
  if (frame) {
    esp_camera_fb_return(frame);     // dispose the buffered image
  }
  delay(100);

  // Take a new photo
  frame = NULL;
  frame = esp_camera_fb_get();

  digitalWrite(pin_led_flash, LOW);     // Turn off the LED Flash after taking photos.

  if (frame)
  {
    Serial.println("Kirim Foto ke Telegram");
    Serial.print("Photo size: ");
    Serial.println(frame->len);
    
    bool sent = bot.sendFile((byte*)frame->buf, frame->len, FB_PHOTO, "photo.jpg", CHAT_ID);
    
    if (sent) {
      Serial.println("Foto Telah Terkirim");
    } else {
      Serial.println("Gagal kirim foto!");
    }
    
    esp_camera_fb_return(frame);
  }
  else
  {
    Serial.println("Camera Capture Error");
  }
  
  // Reconnect MQTT after photo
  delay(500);
  connectMQTT();
}