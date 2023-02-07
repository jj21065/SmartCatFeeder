/*
 本範例為《超圖解物聯網IoT實作入門》書籍補充內容。
 相關說明請參閱：
 http://swf.com.tw/?p=911
*/
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <FS.h>

const byte LED_PIN = 2;
const byte PWM_PIN = 0;

const char ssid[] = "JMhouse";
const char pass[] = "0926621065";
const char *host = "laifuHost";

ESP8266WebServer server(80);

// wifi manager
#define AP_SSID "JarvisAP" // 自訂的ESP裝置AP名稱
#define AP_PWD "12345678"  // 自訂的AP密碼
#define TRIGGER_PIN 0      // 啟用「Wi-Fi設置入口」的按鍵接腳

WiFiManager wm; // 建立WiFi管理員物件

unsigned int timeout = 120; // Wi-Fi管理員的運作秒數
unsigned int startTime = millis();
bool portalRunning = false;
bool startAP = true; // 僅啟動網站伺服器，設成true則啟動AP和網站伺服器。
/// end wifi manager

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

void WifiMangagerInitial()
{
    WiFi.mode(WIFI_STA); // Wi-Fi設置成STA模式；預設模式為STA+AP

    Serial.setDebugOutput(true);
    delay(1000);
    pinMode(TRIGGER_PIN, INPUT_PULLUP);

    wm.setHostname(host); // 設置ESP的主機名稱
    // wm.setDebugOutput(false);  // 關閉除錯訊息
    wm.autoConnect(AP_SSID, AP_PWD);
    // 確認晶片是否存有Wi-Fi連鍵資料
    if (WiFi.status() == WL_CONNECTED && wm.getWiFiIsSaved())
    {
        Serial.println("\n晶片存有Wi-Fi連線資料！");
    }
    else
    {
        Serial.println("\n晶片沒有Wi-Fi連線資料…");
    }
    // 設定我們自訂的網站伺服器
    //server.on("/", []() { // 處理”/”路徑的路由
    //    server.send(200, "text/html; charset=utf-8", "人生最大的風險，<br>就是不願意冒險。");
    //});
    server.onNotFound([]() { // 處理「找不到指定資源」的路由
        server.send(404, "text/plain", "File NOT found!");
    });

    server.begin(); // 啟動網站伺服器
}

void WebServerInitial()
{
    pinMode(LED_PIN, OUTPUT);

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
     if (state == "ON") {
         digitalWrite(LED_PIN, HIGH);
     } else if (state == "OFF") {
         digitalWrite(LED_PIN, LOW);
     }
     
     Serial.print("LED_PIN: ");
     Serial.println(state); });

    server.on("/pwm", []()
              {
     String pwm = server.arg("led");
     int val = pwm.toInt();
     //analogWrite(PWM_PIN, val);
     Serial.print("PWM: ");
     Serial.println(val); });

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

void doWiFiManager()
{
    if (portalRunning)
    {
        wm.process();
        if ((millis() - startTime) > (timeout * 1000))
        {
            Serial.println("「Wi-Fi設置入口」操作逾時…");
            portalRunning = false;

            if (startAP)
            {
                wm.stopConfigPortal();
            }
            else
            {
                wm.stopWebPortal();
            }

            server.begin(); // 再次啟動我們的網站伺服器
        }
    }

    // 若啟用「Wi-Fi設置入口」的接腳被按一下…
    if (digitalRead(TRIGGER_PIN) == LOW && (!portalRunning))
    {
        server.stop(); // 停止我們自訂的網站伺服器程式

        if (startAP)
        {
            Serial.println("按鈕被按下了，啟動設置入口。");
            wm.setConfigPortalBlocking(false);
            wm.startConfigPortal(AP_SSID, AP_PWD);
        }
        else
        {
            Serial.println("按鈕被按下了，啟動Web入口。");
            wm.startWebPortal();
        }
        portalRunning = true;
        startTime = millis();
    }
}

void WebServerHandler()
{
    server.handleClient();
}

void setup()
{
  
    Serial.begin(115200);
  WifiMangagerInitial();
  WebServerInitial();
}

void loop(){
  doWiFiManager();
  WebServerHandler();
}
