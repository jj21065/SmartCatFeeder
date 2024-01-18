#ifndef SCALERWIFICONTROL_H
#define SCALERWIFICONTROL_H
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266mDNS.h>
#include <FS.h>
#include "HTTPSRedirect/HTTPSRedirect.h"
#include "../../../PinshareSetting.h"

// wifi manager
#define AP_SSID "JarvisAP" // 自訂的ESP裝置AP名稱
#define AP_PWD "12345678"  // 自訂的AP密碼

const char ssid[] = "JMhouse";
const char pass[] = "0926621065";
extern const char *host;        // = "laifuHost";
extern ESP8266WebServer server; //(80);
void ScalerWifiMangagerInitial();

void ScalerDoWiFiManager();

void ScalerWebServerHandler();

void InitialWifiSearch();

String HttpPost(char *httphost, String dataString);

String HttpGet(String httphost);

String DecodeJson(const String payload, String key);

String HttpGetGoogleScriptSecure(String url);

String HttpPostGoogleScriptSecure(String url, String payload);
#endif