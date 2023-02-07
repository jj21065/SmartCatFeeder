#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>

StaticJsonDocument<200> jsondoc;
char json_output[100];

const char* ssid = "XXXXX"; //WIFI SSID
const char* password = "XXXXX";//WIFI PASSWORD

AsyncWebServer server(80); //PORT號
AsyncWebSocket ws("/ws");

//每當我們通過 WebSocket 協議從客戶端接收到新數據時，它就會運行
void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
 
    String catchvalue = String((char*)data);
    Serial.println(catchvalue);//顯示收到的數值
  }
       
}

//配置一個事件監聽器來處理 WebSocket 協議的不同異步步驟。這個事件處理程序可以通過定義onEvent()
void BonEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      handleWebSocketMessage(arg, data, len);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}

void notifyClients() {

    const uint8_t size = JSON_OBJECT_SIZE(3);
    StaticJsonDocument<size> jsondoc;
    jsondoc["room"] = "chosemaker";
    jsondoc["rand"] = random(0,40);

    char data[100];
    size_t len = serializeJson(jsondoc, data);
    ws.textAll(data, len);
  
}

void initWebSocket() {//初始化 WebSocket 協議
  ws.onEvent(BonEvent);
  server.addHandler(&ws);
}

void setup() {
  Serial.begin(115200);

  if (!SPIFFS.begin(true)) {
    Serial.println("掛載SPIFFS分區出錯啦～");
    return;
  }

  WiFi.mode(WIFI_STA);//SEVER用途選STA,雖然預設也是默認STA
  WiFi.begin(ssid, password);
  Serial.println("");

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.print("\nIP位址：");
  Serial.println(WiFi.localIP());
  
  server.serveStatic("/", SPIFFS, "/www/").setDefaultFile("index.html");

  initWebSocket();


  // Web Server Root URL
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", "text/html");
  });

  server.begin();
  Serial.println("HTTP伺服器開工了～");
}

void loop() {
  delay(500);
  ws.cleanupClients();
  notifyClients();//發送給CLIENT
}
