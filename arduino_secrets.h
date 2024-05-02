#ifndef __ARDUINO_SECRETS__H__
#define __ARDUINO_SECRETS__H__

const char* SECRET_SERVER_VERSION_UPDATE_URL = "http://your-appengine-site.appspot.com/setkeyvalue?id=esp_version&k=";
const char* SECRET_SERVER_BATTERY_UPDATE_URL = "http://your-appengine-site.appspot.com/setkeyvalue?id=battery&k=";
const char* SECRET_NIGHTSCOUT_ENTRIES_URL = "https://your-nightscout-url/api/v1/entries.json?count=1";

const char* SECRET_WIFI_SSID = "SSID";
const char* SECRET_WIFI_PSK = "PSK";
#endif