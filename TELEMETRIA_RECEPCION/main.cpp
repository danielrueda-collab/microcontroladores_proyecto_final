#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include "SPIFFS.h" // Para leer la carpeta 'data'

const char* ssid = "Mao";
const char* password = "123cuatro";

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

void setup() {
    Serial.begin(115200);

    if(!SPIFFS.begin(true)){ 
        Serial.println("Error SPIFFS"); 
        return; 
    }

    // La ESP32 crea su propia red WiFi
    const char* local_ssid = "Telemetria_Cohete";
    const char* local_pass = "12345678";
    
    WiFi.softAP(local_ssid, local_pass);

    IPAddress IP = WiFi.softAPIP();

    Serial.println("Servidor creado correctamente!");
    Serial.print("Conéctate a la red: ");
    Serial.println(local_ssid);
    Serial.print("IP del servidor: ");
    Serial.println(IP); // Normalmente será 192.168.4.1

    server.addHandler(&ws);

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(SPIFFS, "/index.html", "text/html");
    });

    server.serveStatic("/", SPIFFS, "/");

    server.begin();
}

void loop() {
    ws.cleanupClients();

    static unsigned long lastTime = 0;

    if (millis() - lastTime > 200) { // Enviar datos cada 200 ms
        lastTime = millis();
        
        StaticJsonDocument<200> doc;

        doc["temp"] = random(20, 30);
        doc["acel"] = random(0, 500) / 100.0;
        doc["alt"]  = random(0, 1000);
        doc["baro"] = random(950, 1050);

        String jsonString;
        serializeJson(doc, jsonString);

        ws.textAll(jsonString);
    }
}
