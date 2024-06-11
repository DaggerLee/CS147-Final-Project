#include <Arduino.h>
#include <EasyUltrasonic.h>
#include <HttpClient.h>
#include <WiFi.h>
#include <Wire.h>
#include <inttypes.h>
#include <stdio.h>

#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs.h"
#include "nvs_flash.h"

const int buzzerPin = 27;
const int ledPin = 25;
float distance1, distance2;
EasyUltrasonic sensor1, sensor2;

char ssid[50] = "wyyxxxxx 2.4g";   // your network SSID (name)
char pass[50] = "SIZHAIzhenexin";  // your network password (use for WPA, or use
                                   // as key for WEP)
const int kNetworkDelay = 1000;
const int kNetworkTimeout = 30 * 1000;

void nvs_access() {
  // Initialize NVS
  esp_err_t err = nvs_flash_init();
  if (err == ESP_ERR_NVS_NO_FREE_PAGES ||
      err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    // NVS partition was truncated and needs to be erased
    // Retry nvs_flash_init
    ESP_ERROR_CHECK(nvs_flash_erase());
    err = nvs_flash_init();
  }
  ESP_ERROR_CHECK(err);

  // Open
  Serial.printf("\n");
  Serial.printf("Opening Non-Volatile Storage (NVS) handle... ");
  nvs_handle_t my_handle;
  err = nvs_open("storage", NVS_READWRITE, &my_handle);
  if (err != ESP_OK) {
    Serial.printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
  } else {
    Serial.printf("Done\n");
    Serial.printf("Retrieving SSID/PASSWD\n");
    size_t ssid_len;
    size_t pass_len;
    err = nvs_get_str(my_handle, "ssid", ssid, &ssid_len);
    err |= nvs_get_str(my_handle, "pass", pass, &pass_len);
    switch (err) {
      case ESP_OK:
        Serial.printf("Done\n");
        Serial.printf("SSID = %s\n", ssid);
        Serial.printf("PASSWD = %s\n", pass);
        break;
      case ESP_ERR_NVS_NOT_FOUND:
        Serial.printf("The value is not initialized yet!\n");
        break;
      default:
        Serial.printf("Error (%s) reading!\n", esp_err_to_name(err));
    }
  }
  // Close
  nvs_close(my_handle);
}

void setup() {
  Serial.begin(9600);

  pinMode(buzzerPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  ledcSetup(0, 1000, 8);
  ledcAttachPin(buzzerPin, 0);

  // nvs_access();
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  sensor1.attach(12, 13);
  sensor2.attach(32, 33);
}

void loop() {
  distance1 = sensor1.getDistanceCM();
  distance2 = sensor2.getDistanceCM();
  float distance = 0;
  if (distance1 >= distance2) {
    distance = distance2;
  } else {
    distance = distance1;
  }

  if (distance > 50) {
    ledcWrite(0, 0);
    digitalWrite(ledPin, LOW);
  } else if (distance > 30 && distance <= 50) {
    ledcWriteTone(0, 250);
    digitalWrite(ledPin, LOW);
  } else if (distance > 10 && distance <= 30) {
    ledcWriteTone(0, 500);
    digitalWrite(ledPin, HIGH);
  } else if (distance <= 10) {
    ledcWriteTone(0, 750);
    digitalWrite(ledPin, HIGH);
  }

  int err = 0;
  WiFiClient c;
  HttpClient http(c);
  char url[150];
  String warning = "";

  if (distance <= 30) {
    Serial.println(" !!!!!");
    warning = "!!!!!";
  }

  snprintf(url, sizeof(url), "/?distance1=%f&distance2=%f", distance1,
           distance2);

  err = http.get("13.57.6.62", 5000, url, NULL);
  if (err == 0) {
    Serial.println("startedRequest ok");
    err = http.responseStatusCode();
    if (err >= 0) {
      Serial.print("Got status code: ");
      Serial.println(err);
      err = http.skipResponseHeaders();
      if (err >= 0) {
        int bodyLen = http.contentLength();
        Serial.print("Content length is: ");
        Serial.println(bodyLen);
        Serial.println();
        Serial.println("Body returned follows:");
        unsigned long timeoutStart = millis();
        char c;
        while ((http.connected() || http.available()) &&
               ((millis() - timeoutStart) < kNetworkTimeout)) {
          if (http.available()) {
            c = http.read();
            Serial.print(c);
            bodyLen--;
            timeoutStart = millis();
          } else {
            delay(kNetworkDelay);
          }
        }
      } else {
        Serial.print("Failed to skip response headers: ");
        Serial.println(err);
      }
    } else {
      Serial.print("Getting response failed: ");
      Serial.println(err);
    }
  } else {
    Serial.print("Connect failed: ");
    Serial.println(err);
  }
  delay(500);  // Adjust delay as needed
}
