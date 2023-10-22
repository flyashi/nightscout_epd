
#include <Arduino.h>

#include "esp32/rom/rtc.h"
#include <HTTPClient.h>
#include <WiFiClient.h>
#include <WiFi.h>
#include "autoupdate.h"

extern const char* SECRET_SERVER_VERSION_UPDATE_URL;
extern const char* SECRET_SERVER_BATTERY_UPDATE_URL;
//extern char* macAddr;
extern const int FW_VERSION;

void set_date_from_server_date_header(const char* date_header) {
	struct tm tm;
        memset(&tm, 0, sizeof(tm));
        //char *retp = strptime("Thu, 05 Oct 2023 21:24:40 GMT", "%a, %d %b %Y %H:%M:%S GMT", &tm);
        char *retp = strptime(date_header, "%a, %d %b %Y %H:%M:%S GMT", &tm);
	time_t timet = mktime(&tm);
	timeval tv;
	tv.tv_sec = timet;
	tv.tv_usec = 0;
	settimeofday(&tv, 0);

}

void update_server_battery(char* macAddr, int v) {

  Serial.printf("Updating server with battery level v=%d...", v);
  String updateURL = String(SECRET_SERVER_BATTERY_UPDATE_URL);
  updateURL.concat(macAddr);
  updateURL.concat("&v=");
  updateURL.concat(v);
  HTTPClient httpClient;
  WiFiClient client;

  httpClient.begin(client, updateURL);

  int httpCode = httpClient.GET();
  Serial.print("Done. Status code = ");
  Serial.println(httpCode);
  if (httpCode == 200) {
    Serial.println(httpClient.getString());
  }
}


void update_server_cache_version(char* macAddr) {
  Serial.println("Updating server with running version...");
  String updateURL = String(SECRET_SERVER_VERSION_UPDATE_URL);
  updateURL.concat(macAddr);
  updateURL.concat("&v=");
  updateURL.concat(FW_VERSION);
  HTTPClient httpClient;
  WiFiClient client;

  httpClient.begin(client, updateURL);

  const char *headerKeys[] = {"Date"};
  const size_t headerKeysCount = sizeof(headerKeys) / sizeof(headerKeys[0]);
    httpClient.collectHeaders(headerKeys, headerKeysCount);
        
  int httpCode = httpClient.GET();
  Serial.print("Done. Status code = ");
  Serial.println(httpCode);
  if (httpCode == 200) {
    Serial.println(httpClient.getString());

    String date = httpClient.header("Date");
    Serial.print("Got Date header: ");
    Serial.println(date);
    set_date_from_server_date_header(date.c_str());
  }
}