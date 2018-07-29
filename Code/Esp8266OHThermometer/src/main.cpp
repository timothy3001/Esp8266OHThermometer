#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "settings.h"

extern "C" {
    #include "user_interface.h"
}

// Necessary objects
OneWire oneWire(ONEWIRE_BUS);
DallasTemperature sensors(&oneWire);
HTTPClient http;

// Declaration of functions
void setupWifi();
void readAndSendTemperature();
double getTemperature();
char* concat(const char *s1, const char *s2);

void setup() 
{
    Serial.begin(115200);
    sensors.begin();
    setupWifi();
}

void loop() 
{
    readAndSendTemperature();
    delay(UPDATE_RATE * 1000);
}


void setupWifi()
{   
    wifi_station_set_hostname(concat("Esp-Thermometer-", WiFi.macAddress().c_str()));
    WiFi.mode(WIFI_STA);
    
    // Enable light sleep for less power consumptino.  NOTE: Currently, LIGHT_SLEEP_T does not seem to work:
    // https://github.com/esp8266/Arduino/issues/4485 

    // wifi_set_sleep_type(LIGHT_SLEEP_T);    

    WiFi.begin(WIFI_AP, WIFI_KEY);

    while(WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.println("Waiting for connection to access point...");
    }  
}

void readAndSendTemperature() 
{
    sensors.requestTemperatures();
    if (sensors.getDeviceCount() > 0)
    {
        double temperature = sensors.getTempCByIndex(0);

        Serial.print("Temperature: ");
        Serial.print(temperature);
        Serial.println("°C");

        http.begin(OH_ITEM_ADDRESS + String("/state"));
        http.addHeader("Content-Type", "text/plain");
        int returnCode = http.PUT(String(temperature, 4));

        Serial.print("HTTP code: ");
        Serial.println(returnCode);
    }
    else
    {
        Serial.println("No temperature sensor found!");
    }
}

char* concat(const char *s1, const char *s2)
{
    char *result = new char[(strlen(s1) + strlen(s2) + 1)]; // +1 for the null-terminator
    // in real code you would check for errors in malloc here
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}