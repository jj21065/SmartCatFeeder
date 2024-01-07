#include <Arduino.h>
#include <Arduino_JSON.h>
#include "ScalerWifiControl.h"

WiFiManager wm; // 建立WiFi管理員物件

unsigned int timeout = 120; // Wi-Fi管理員的運作秒數
unsigned int startTime = millis();
bool portalRunning = false;
bool startAP = true; // 僅啟動網站伺服器，設成true則啟動AP和網站伺服器。
/// end wifi manager
extern const char *host = "laifuHost";
extern ESP8266WebServer server(1025);

void ScalerWifiMangagerInitial()
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

void ScalerDoWiFiManager()
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

void ScalerWebServerHandler()
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

void HttpPost(char *httphost)
{
  if ((WiFi.status() == WL_CONNECTED))
  {

    WiFiClient client;
    HTTPClient http;

    Serial.print("[HTTP] begin...\n");
    // configure traged server and url
    http.begin(client, httphost); // HTTP
    http.addHeader("Content-Type", "application/json");

    Serial.print("[HTTP] POST...\n");
    // start connection and send HTTP header and body
    // int httpCode = http.POST("{\"hello\":\"world\"}");
    int httpCode = http.POST("");
    // httpCode will be negative on error
    if (httpCode > 0)
    {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTP] POST... code: %d\n", httpCode);

      // file found at server
      if (httpCode == HTTP_CODE_OK)
      {
        const String &payload = http.getString();
        Serial.println("received payload:\n<<");
        Serial.println(payload);
        Serial.println(">>");
        DecodeJson(payload);
      }
    }
    else
    {
      Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
  }
}

String HttpGet(char *httphost)
{
  HTTPClient http;
  WiFiClient client;
  String payload = "fail";
  http.setTimeout(10000);
  // Send HTTP GET request to WorldTimeAPI
  http.begin(client, httphost);
  int httpCode = http.GET();
  Serial.println(httpCode);
  if (httpCode > 0)
  {

    if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY)
    {
      payload = http.getString();
      Serial.println(payload);
      // Serial.println("get time.");
    }
  }
  else
  {
    Serial.println("Failed to connect to API");
  }

  http.end();
  return payload;
}

void DecodeJson(const String payload)
{
  JSONVar myObject = JSON.parse(payload);
  String decodeArr[5];
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
    decodeArr[i] = String(value);
  }
  Serial.print("1 = ");
  Serial.println(decodeArr[0]);
  Serial.print("2 = ");
  Serial.println(decodeArr[1]);
  Serial.print("3 = ");
  Serial.println(decodeArr[2]);
}
// search for wifi name (test, not yet ready)
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
