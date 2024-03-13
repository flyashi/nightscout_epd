#include "nightscout.h"
#include <HTTPClient.h>
#include <ArduinoJson.h>

extern const char* SECRET_NIGHTSCOUT_ENTRIES_URL;
extern int sgv;
extern int sgv_delta;
extern long sgv_ts;

bool update_nightscout() {
  
             HTTPClient http;
              http.begin(SECRET_NIGHTSCOUT_ENTRIES_URL);
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

bool update_nightscout_local(String gateway_ip) {
  
            String url="http://";
            url.concat(gateway_ip);
            url.concat(":17580/sgv.json?count=1");
             HTTPClient http;
              http.begin(url);
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