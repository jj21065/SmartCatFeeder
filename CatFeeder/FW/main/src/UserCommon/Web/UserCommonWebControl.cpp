#include <Arduino.h>
#include <stdlib.h>
#include <Arduino_JSON.h>

#include "UserCommonWebControl.h"
#include "../../Scaler/ScalerInclude.h"
#include "../Extruder/UserCommonExtruder.h"
// const byte LED_PIN = 2;
const byte PWM_PIN = 0;
// GET/ POST API
void PostUpdateFeed()
{
    String payload = server.arg(0);
    // UserCommonExtruderRun();
    JSONVar myObject = JSON.parse(payload);

    // JSON.typeof(jsonVar) can be used to get the type of the var
    if (JSON.typeof(myObject) == "undefined")
    {
        Serial.println("Parsing input failed!");
        return;
    }

    Serial.print("JSON object = ");
    Serial.println(myObject);

    // myObject.keys() can be used to get an array of all the keys in the object
    JSONVar keys = myObject.keys();
    if (keys.length() > 0)
    {
        JSONVar value = myObject["amount"];
        server.send(200, "text/plain", "POST body was:\n" + server.arg("plain"));
    }

    UserCommonManualFeedOutput(((String)value).toInt());
}

void PostUpdateFeedSchedule()
{
    String payload = server.arg(0);
    // DecodeJson(payload);
    JSONVar myObject = JSON.parse(payload);

    // JSON.typeof(jsonVar) can be used to get the type of the var
    if (JSON.typeof(myObject) == "undefined")
    {
        Serial.println("Parsing input failed!");
        return;
    }

    Serial.print("JSON object = ");
    Serial.println(myObject);

    // myObject.keys() can be used to get an array of all the keys in the object
    JSONVar keys = myObject.keys();

    for (int i = 0; i < keys.length(); i++)
    {
        JSONVar value = myObject[keys[i]];
        Serial.print(keys[i]);
        Serial.print(" = ");
        Serial.println(value);
        // decodeArr[i] = String(value);
    }

    server.send(200, "text/plain", "POST body was:\n" + server.arg("plain"));
}
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
        Serial.println("file found: " + pathWithGz);
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
                // if(state == "ON")
                //     UserCommonTestDigitalOutput(true);
                // else
                //     UserCommonTestDigitalOutput(false);
                Serial.println(state); });
    server.on("/feed", PostUpdateFeed);
    server.on("/feedSchedule", PostUpdateFeedSchedule);

    // 處理根路徑以及「不存在的」路徑
    server.onNotFound([]()
                      {
        if (!handleFileRead(server.uri()))
            server.send(404, "text/plain", "FileNotFound"); });

    server.begin();
    Serial.println("HTTP server started");

    MDNS.setInstanceName("Jake's ESP8266");
    MDNS.addService("http", "tcp", 80);
}

void UserCommonWebCurrentTime()
{
    String payload = HttpGet("http://worldtimeapi.org/api/timezone/Asia/Taipei");
    // Parse the JSON response to get the UTC datetime
    // Note: You may need to use a JSON parsing library for more robust parsing
    int startIndex = payload.indexOf("\"datetime\":\"") + 12;
    int endIndex = payload.indexOf("\"day_of_week\"");
    String currentDateTime = payload.substring(startIndex, endIndex);

    Serial.println("Current DateTime: " + currentDateTime);
    // Current DateTime : "," client_ip ":" 36.230.186.98 "," datetime ":" 2023 - 12 - 30T12 : 06 : 00.090198 + 08 : 00 "," day_of_week ":6," day_of_year ":364," dst ":false," dst_from ":null," dst_offset ":0," dst_until ":null," raw_offset ":28800," timezone ":" Asia / Taipei "," unixtime ":1703909160," utc_datetime ":"
}

void UserCommonWebServerHandler()
{
    ScalerDoWiFiManager();
    ScalerWebServerHandler();
    // getCurrentTime();
}