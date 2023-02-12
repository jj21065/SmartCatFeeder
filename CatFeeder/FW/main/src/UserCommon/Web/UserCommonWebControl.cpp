#include <Arduino.h>
#include <stdlib.h>
#include "UserCommonWebControl.h"
#include "../../Scaler/ScalerInclude.h"
#include "../Extruder/UserCommonExtruder.h"
// const byte LED_PIN = 2;
const byte PWM_PIN = 0;

// 定義處理首頁請求的自訂函式
String getContentType(String filename)
{
    if (server.hasArg("download"))
        return "application/octet-stream";
    else if (filename.endsWith(".htm"))
        return "text/html";
    else if (filename.endsWith(".html"))
        return "text/html";
    else if (filename.endsWith(".css"))
        return "text/css";
    else if (filename.endsWith(".js"))
        return "application/javascript";
    else if (filename.endsWith(".png"))
        return "image/png";
    else if (filename.endsWith(".gif"))
        return "image/gif";
    else if (filename.endsWith(".jpg"))
        return "image/jpeg";
    else if (filename.endsWith(".ico"))
        return "image/x-icon";
    else if (filename.endsWith(".xml"))
        return "text/xml";
    else if (filename.endsWith(".pdf"))
        return "application/x-pdf";
    else if (filename.endsWith(".zip"))
        return "application/x-zip";
    else if (filename.endsWith(".gz"))
        return "application/x-gzip";
    return "text/plain";
}

bool handleFileRead(String path)
{
    Serial.println("handleFileRead: " + path);
    if (path.endsWith("/"))
        path += "index.htm";
    String contentType = getContentType(path);
    String pathWithGz = path + ".gz";
    if (SPIFFS.exists(pathWithGz) || SPIFFS.exists(path))
    {
        if (SPIFFS.exists(pathWithGz))
            path += ".gz";
        File file = SPIFFS.open(path, "r");
        size_t sent = server.streamFile(file, contentType);
        file.close();
        return true;
    }
    return false;
}

void UserCommonWebServerProcess(String state)
{
    if (state == "ON")
    {
        // UserCommonExtruderPush(180);
        //  digitalWrite(LED_PIN, HIGH);
    }
    else if (state == "OFF")
    {
        // UserCommonExtruderExtract(-90);
        // digitalWrite(LED_PIN, LOW);
    }
}

void UserCommonWebServerSettingInitial()
{
    ScalerWifiMangagerInitial();
    // pinMode(LED_PIN, OUTPUT);

    SPIFFS.begin(); // 啟用SPIFFS檔案系統

    WiFi.begin(ssid, pass);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    IPAddress ip = WiFi.localIP();

    if (!MDNS.begin(host, ip))
    {
        Serial.println("Error setting up MDNS responder!");
        while (1)
        {
            delay(1000);
        }
    }
    Serial.println("mDNS responder started");

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.print("Connected! IP address: ");
    Serial.println(WiFi.localIP());

    server.on("/sw", []()
              {
                String state = server.arg("led");
                // UserCommonExtruderRun();
                Serial.println(state); });
    server.on("/feed", []()
              {
                String state = server.arg("feed");
                UserCommonExtruderRun();
                Serial.print("feed state: ");
                Serial.println(state); });

    server.on("/foodAmount", []()
              {
                  String pwm = server.arg("food");
                  int val = pwm.toInt();
                  UserCommonExtruderSetValue(val);

                  // analogWrite(PWM_PIN, val);
              });

    // 處理根路徑以及「不存在的」路徑
    server.onNotFound([]()
                      {
    if(!handleFileRead(server.uri()))
      server.send(404, "text/plain", "FileNotFound"); });

    server.begin();
    Serial.println("HTTP server started");

    MDNS.setInstanceName("Cubie's ESP8266");
    MDNS.addService("http", "tcp", 80);
}

void UserCommonWebServerHandler()
{
    ScalerDoWiFiManager();
    ScalerWebServerHandler();
}