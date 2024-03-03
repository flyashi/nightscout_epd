#include "nightscout.h"
#include <HTTPClient.h>
#include <ArduinoJson.h>

extern char[100] nightscout_server;
extern int sgv;
extern int sgv_delta;
extern long sgv_ts;

bool update_nightscout() {
  HTTPClient http;
  String entries_url = String(nightscout_server);
  if (!entries_url.endsWith("/")) {
    entries_url += String("/");
  }
  entries_url + String("api/v1/entries.json?count=1");
  http.begin(entries_url.c_str());
  int retcode = http.GET();
  if (retcode != 200) {
    Serial.print("HTTP error: ");
    Serial.println(retcode);
    return false;
  }
  DynamicJsonDocument doc(5000);
  deserializeJson(doc, http.getStream());
  int reading = doc[0]["sgv"];
  float delta = doc[0]["delta"];
  long long reading_ms = doc[0]["mills"];
  long reading_sec = reading_ms / 1000;
  sgv=reading;
  sgv_delta=round(delta);
  sgv_ts=reading_sec;

  return true;
}