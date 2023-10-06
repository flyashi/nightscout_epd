#include "nightscout.h"
#include <HTTPClient.h>

extern const char* SECRET_NIGHTSCOUT_ENTRIES_URL;
extern int sgv;
extern int sgv_delta;
extern long sgv_ts;

bool update_nightscout() {
             HTTPClient http;
              http.begin(SECRET_NIGHTSCOUT_ENTRIES_URL);
              http.GET();
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