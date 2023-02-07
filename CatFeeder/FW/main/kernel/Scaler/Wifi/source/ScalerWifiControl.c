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
  // server.on("/", []() { // 處理”/”路徑的路由
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

void InitialWifiSearch()
{
  Serial.println(F("\nESP8266 WiFi scan example"));

  // Set WiFi to station mode
  WiFi.mode(WIFI_STA);

  // Disconnect from an AP if it was previously connected
  WiFi.disconnect();
  delay(100);
}

// search for wifi name
void kernelWifiSearch()
{
  String ssid;
  int32_t rssi;
  uint8_t encryptionType;
  uint8_t *bssid;
  int32_t channel;
  bool hidden;
  int scanResult;

  Serial.println(F("Starting WiFi scan..."));

  scanResult = WiFi.scanNetworks(/*async=*/false, /*hidden=*/true);

  if (scanResult == 0)
  {
    Serial.println(F("No networks found"));
  }
  else if (scanResult > 0)
  {
    Serial.printf(PSTR("%d networks found:\n"), scanResult);

    // Print unsorted scan results
    for (int8_t i = 0; i < scanResult; i++)
    {
      WiFi.getNetworkInfo(i, ssid, encryptionType, rssi, bssid, channel, hidden);

      // get extra info
      const bss_info *bssInfo = WiFi.getScanInfoByIndex(i);
      String phyMode;
      const char *wps = "";
      if (bssInfo)
      {
        phyMode.reserve(12);
        phyMode = F("802.11");
        String slash;
        if (bssInfo->phy_11b)
        {
          phyMode += 'b';
          slash = '/';
        }
        if (bssInfo->phy_11g)
        {
          phyMode += slash + 'g';
          slash = '/';
        }
        if (bssInfo->phy_11n)
        {
          phyMode += slash + 'n';
        }
        if (bssInfo->wps)
        {
          wps = PSTR("WPS");
        }
      }
      Serial.printf(PSTR("  %02d: [CH %02d] [%02X:%02X:%02X:%02X:%02X:%02X] %ddBm %c %c %-11s %3S %s\n"), i, channel, bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5], rssi, (encryptionType == ENC_TYPE_NONE) ? ' ' : '*', hidden ? 'H' : 'V', phyMode.c_str(), wps, ssid.c_str());
      yield();
    }
  }
  else
  {
    Serial.printf(PSTR("WiFi scan error %d"), scanResult);
  }

  // Wait a bit before scanning again
  delay(5000);
}
