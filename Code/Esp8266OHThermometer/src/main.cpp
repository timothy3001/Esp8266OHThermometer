#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>

extern "C" {
    #include "user_interface.h"
}


// You need to set the following parameters
#define WIFI_AP "[WIFI-SSID]"
#define WIFI_KEY "[WIFI-KEY]"
#define OH_ITEM_ADDRESS "[OPENHAB-ITEM-ADDRESS]" // like http://openhabpi:8080/rest/items/My_Great_Temp_sensor NOTE: Enter the full item address only. No slash at the end!
#define UPDATE_RATE 60 // in seconds

// Should work if you followed the wiring
#define ONEWIRE_BUS 2

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
    wifi_set_sleep_type(LIGHT_SLEEP_T); // Enable light sleep

    WiFi.begin(WIFI_AP, WIFI_KEY);

    while(WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.println("Waiting for connection to access point...");
    }  
}

void readAndSendTemperature() 
{
    double temperature = getTemperature();

    http.begin(OH_ITEM_ADDRESS + String("/state"));
    http.addHeader("Content-Type", "text/plain");
    int returnCode = http.PUT(String(temperature, 4));

    Serial.print("HTTP code: ");
    Serial.println(returnCode);
}

double getTemperature()
{
    sensors.requestTemperatures();
    double temp = sensors.getTempCByIndex(0);

    Serial.print("Temperature: ");
    Serial.print(temp);
    Serial.println("°C");

    return temp;
}

char* concat(const char *s1, const char *s2)
{
    char *result = new char[(strlen(s1) + strlen(s2) + 1)]; // +1 for the null-terminator
    // in real code you would check for errors in malloc here
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}