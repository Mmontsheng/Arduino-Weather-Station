/**
   Author: Mmontsheng Maoto

   Description here

    Created on: 22.06.2021

*/
// comment this and uncomment "YOUR_SECRETS_FILE.h"
#include "SECRETS.h"
// #include "YOUR_SECRETS_FILE.h"

#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

const char ssid[] = WIFI_NAME;
const char pass[] = WIFI_PASSWORD;

const String API_KEY = OPENWEATHERMAP_API_KEY;

//the location you want the weather for
const String MY_LAT = "-25.751626244764665";
const String MY_LON = "28.20914355288296";

int weather = 0;

int status = WL_IDLE_STATUS;

unsigned long lastConnectionTime = 0;            // last time you connected to the server, in milliseconds
const unsigned long postingInterval = 30L * 1000L; // delay between updates, in milliseconds

StaticJsonDocument<768> doc;

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);

    // status = WiFi.begin(ssid);
    //use the line below if your network is protected by wpa password
    status = WiFi.begin(ssid, pass);

    // wait 1 second for connection:
    delay(1000);
  }
  Serial.println("Connected to wifi");

}

void loop() {
  if (millis() - lastConnectionTime > postingInterval)
  {
    httpRequest();
  }
}

void httpRequest() {
  HTTPClient http;
  String url = "http://api.openweathermap.org/data/2.5/weather?lat=" + MY_LAT + "&lon=" + MY_LON + "&appid=" + API_KEY + "&units=metric";

  Serial.println(url);

  http.begin(url); //HTTP
  int httpCode = http.GET();

  // httpCode will be negative on error
  if (httpCode > 0) {
    // HTTP header has been send and Server response header has been handled
    Serial.printf("[HTTP] GET... code: %d\n", httpCode);

    // file found at server
    if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      int payloadSize = payload.length();

      // declaring character array
      char char_array[payloadSize + 1];

      // copying the contents of the
      // string to char array
      strcpy(char_array, payload.c_str());

      weather = parseWeatherResponse(char_array);

      Serial.println(payload);
      Serial.println(weather);
      lastConnectionTime = millis();
    }
  } else {
    Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();
}

int parseWeatherResponse(char* input) {
  DeserializationError error = deserializeJson(doc, input);
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return weather;
  }

  float coord_lon = doc["coord"]["lon"]; // 28.2091
  float coord_lat = doc["coord"]["lat"]; // -25.7516

  JsonObject weather_0 = doc["weather"][0];
  int weather_0_id = weather_0["id"]; // 800
  const char* weather_0_main = weather_0["main"]; // "Clear"
  const char* weather_0_description = weather_0["description"]; // "clear sky"
  const char* weather_0_icon = weather_0["icon"]; // "01d"

  const char* base = doc["base"]; // "stations"

  JsonObject main = doc["main"];
  return main["temp"]; // 13.63
}
