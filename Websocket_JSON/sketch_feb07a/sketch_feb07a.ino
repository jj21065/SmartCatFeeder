#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <FS.h>

const char ssid[] = "你的WiFi網路SSID";
const char pass[] = "你的WiFi密碼";
const char* host = "jarvis";

ESP8266WebServer server(80);

String getContentType(String filename){
  if(server.hasArg("download")) return "application/octet-stream";
  else if(filename.endsWith(".htm")) return "text/html";
  else if(filename.endsWith(".html")) return "text/html";
  else if(filename.endsWith(".css")) return "text/css";
  else if(filename.endsWith(".js")) return "application/javascript";
  else if(filename.endsWith(".png")) return "image/png";
  else if(filename.endsWith(".gif")) return "image/gif";
  else if(filename.endsWith(".jpg")) return "image/jpeg";
  else if(filename.endsWith(".ico")) return "image/x-icon";
  else if(filename.endsWith(".xml")) return "text/xml";
  else if(filename.endsWith(".pdf")) return "application/x-pdf";
  else if(filename.endsWith(".zip")) return "application/x-zip";
  else if(filename.endsWith(".gz")) return "application/x-gzip";
  return "text/plain";
}

bool handleFileRead(String path){
  Serial.println("handleFileRead: " + path); // 在序列埠顯示路徑

  if (path.endsWith("/")) {
    path += "index.htm";
  }

  String contentType = getContentType(path);
  
  if (SPIFFS.exists(path)){
    File file = SPIFFS.open(path, "r");
    server.streamFile(file, contentType);
    file.close();

    return true;
  }
  return false;
}

void setup( ){
  Serial.begin(115200);

  SPIFFS.begin();  // 啟用SPIFFS檔案系統

  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  IPAddress ip = WiFi.localIP();
  
  if (!MDNS.begin(host, ip)) {
    Serial.println("Error setting up MDNS responder!");
    while(1) { 
      delay(1000);
    }
  }
  Serial.println("mDNS responder started");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.print("Connected! IP address: ");
  Serial.println(WiFi.localIP());

  server.onNotFound([](){
    if (!handleFileRead(server.uri())) {
      server.send(404, "text/plain", "FileNotFound");
    }
  });

  server.begin();
  Serial.println("HTTP server started");

  MDNS.setInstanceName("Cubie's ESP8266");
  MDNS.addService("http", "tcp", 80);
}
 
void loop( ){
  server.handleClient();
}
