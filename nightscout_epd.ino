
#include "display.h"
#include "autoupdate.h"
#include "nightscout.h"
#include "arduino_secrets.h"
#include "kvstore_client.h"

extern const int FW_VERSION;

#include <Preferences.h>

Preferences preferences;


int wifi_signal;
extern const char* FW_URL_BASE;
void print_time();
#include <Arduino.h>

#include <WiFiClient.h>
#include <HTTPClient.h>

#include <WiFi.h>
#include <WiFiManager.h>
#if USE_WIFIMANAGER

WiFiManager wifiManager;
#endif


uint8_t mac[6];
char macAddr[14];

int sgv;
int sgv_delta;
long sgv_ts;
long prev_sgv_ts;
boolean UpdateLocalTime();
#if ESP32
#include "esp32/rom/rtc.h"
#elif ESP32S3
#include "esp_system.h"
#endif

void print_reset_reason(int);
void print_time();
void set_date_from_server_date_header(const char* date_header);

int get_battery_mv() {
  // read slowly to avoid noise
  analogSetClockDiv(255);
  //int v = analogRead(35);
  int v = analogReadMilliVolts(35);
  // raw millivolts is half of actual battery voltage. multiply by two to get actual voltage
  v = v * 2;
  return v;
}

void configModeCallback (WiFiManager *myWiFiManager) {
#if USE_WIFIMANAGER

  Serial.println("Entered config mode - wifi failed to connect");

#if ESP32
  // decide whether to continue into AP mode, or just sleep and retry in 1min
  if (rtc_get_reset_reason(0) != 1) {
    Serial.println("Not a power-on reset, so sleeping for 1min");
    esp_sleep_enable_timer_wakeup(60*1000000);
    esp_deep_sleep_start();
  }
#else
  Serial.println("Non-ESP32 not supported for config mode callback");
#endif

  Serial.println(WiFi.softAPIP());

  Serial.println(myWiFiManager->getConfigPortalSSID());
  char buf[100];
  sprintf(buf, "AP: %s", myWiFiManager->getConfigPortalSSID().c_str());
  fullscreen_message(buf);
#endif

}

uint8_t init_wifi() {
  WiFi.macAddress(mac);
  sprintf(macAddr, "%02X%02X%02X%02X%02X%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  Serial.print("This device's MAC address: ");
  Serial.println(macAddr);

  Serial.print("This device's MAC address: ");
  Serial.println(macAddr);
  String s = String(FW_URL_BASE);
  s.concat(macAddr);
  s.concat(".version");

  Serial.print("Place the version file here: ");
  Serial.println(s);
  String s2 = String(FW_URL_BASE);
  s2.concat(macAddr);
  s2.concat(".");
  s2.concat(FW_VERSION);
  s2.concat(".bin");
  Serial.print("Place the firmware file e.g. here: ");
  Serial.println(s2);

#if USE_WIFIMANAGER
  wifiManager.setAPCallback(configModeCallback);

  //wifiManager.setConfigPortalTimeout(300);
  return wifiManager.autoConnect();
#else
  // in case of hidden SSID on channel 13
  // https://github.com/espressif/esp-idf/issues/2989#issuecomment-459941899
  // linked from
  // https://github.com/espressif/arduino-esp32/issues/3961#issuecomment-624740732
  wifi_country_t country = {
    .cc = "CN",
    .schan = 1,
    .nchan = 13,
    .policy = WIFI_COUNTRY_POLICY_MANUAL,
  };
  esp_wifi_set_country(&country);
  WiFi.begin(SECRET_WIFI_SSID, SECRET_WIFI_PSK);
  int retries = 15;
  while (WiFi.status() != WL_CONNECTED && retries > 0) {
    delay(500);
    Serial.print(".");
    retries--;
  }
  return WiFi.status() == WL_CONNECTED;
#endif
}

void xkcd_434() {
  Serial.println("disabling wifi...");
  WiFi.disconnect();
  WiFi.mode(WIFI_OFF);
  Serial.println("disabled");
}
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Ready");
#if TEST
  delay(5000);
#else
  delay(10);
#endif

#if ESP32
  Serial.println("CPU0 reset reason:");
  print_reset_reason(rtc_get_reset_reason(0));
  delay(10);
#elif ESP32S3
  Serial.println("ESP32S3 to be implemented");
#else
  Serial.println("Don't know how to get reset reason");
#endif
  print_time();
  delay(10);
#if TEST
Serial.println("display init");
  display_init();
  Serial.println("sleep");
  sleep(1000);
  Serial.println("display hibernate");
  display_hibernate();
  Serial.println("set sleep timer and deep sleep");
          esp_sleep_enable_timer_wakeup(5*1000000);
        esp_deep_sleep_start();
        Serial.println("did not sleep");

#endif
  preferences.begin("nightscout_epd", false);
#if ESP32
  if (rtc_get_reset_reason(0) == 1) {
    preferences.remove("prev_sgv_ts");

    fullscreen_message("Welcome:)");
  }
#else
  Serial.println("Non-ESP32 not implemented");
#endif
  if (init_wifi()) {
    // char buf[100];
    // sprintf(buf, "%s", macAddr);
    // HelloWorld = buf;
    // display_init();
    // helloWorld();
    // display_hibernate();
    // delay(1000);

    //update_server_cache_version(macAddr);
    //check_for_remote_update(macAddr);
    int battery_mv = get_battery_mv();
    //update_server_battery(macAddr, battery_mv);

    bool local_successful = update_nightscout_local(WiFi.gatewayIP().toString());
    bool remote_successful = false;
    if (!local_successful) {
      remote_successful = update_nightscout();
    }
    if (local_successful || remote_successful) {
      char buf[100];
      prev_sgv_ts = preferences.getLong64("prev_sgv_ts", 0);
      if (sgv_ts != prev_sgv_ts || 
      /* rtc_get_reset_reason(0) <= 3 */0) {
        Serial.printf("Got new data from nightscout, or hard/soft reset; old ts=%ld new ts=%ld sgv=%d sgv_delta=%d", prev_sgv_ts, sgv_ts, sgv, sgv_delta);
        if (sgv_delta >= 0) 
          sprintf(buf, "%d +%d", sgv, sgv_delta);
        else
          sprintf(buf, "%d %d", sgv, sgv_delta);
        char tsbuf[100];
        time_t now;
        struct tm timeinfo;
        //time(&now);
        now = sgv_ts;
        localtime_r(&now, &timeinfo);

        strftime(tsbuf, sizeof(tsbuf), "%Y-%m-%d %H:%M:%S", &timeinfo);

        //fullscreen_message_subtitle(buf, tsbuf);
        char headbuf[100];
        sprintf(headbuf, "battery: %d", battery_mv);
        fullscreen_message_subtitle_header(buf, tsbuf, headbuf);
        preferences.putLong64("prev_sgv_ts", sgv_ts);
        xkcd_434();
        esp_sleep_enable_timer_wakeup(60*1000000);
        esp_deep_sleep_start();
      } else {
        Serial.printf("No new data from nightscout, and not hard/soft reset; old ts=%ld new ts=%ld sgv=%d sgv_delta=%d\n", prev_sgv_ts, sgv_ts, sgv, sgv_delta);

        //sprintf(buf, "No new data");
        // fullscreen_message(buf);
        xkcd_434();
        esp_sleep_enable_timer_wakeup(60*1000000);
        esp_deep_sleep_start();
      }
    } else {
      Serial.println("Could not get data from nightscout. deep sleep for 1 minute.");
      // delay(5000);
      // ESP.restart();
      xkcd_434();
      esp_sleep_enable_timer_wakeup(60*1000000);
      esp_deep_sleep_start();
    }
  } else {
    // wifi failed to connect. deep sleep for 1 minute.
    Serial.println("wifi failed to connect. deep sleep for 1 minute.");
    // char buf[100];
    // sprintf(buf, "Restarting...");
    // HelloWorld = buf;
    // helloWorld();
    // delay(5000);
    // ESP.restart();
    xkcd_434();
    esp_sleep_enable_timer_wakeup(60*1000000);
    esp_deep_sleep_start();
  }
}


void loop() { };


void print_reset_reason(int reason)
{
  switch ( reason)
  {
    case 1 : Serial.println ("POWERON_RESET");break;          /**<1,  Vbat power on reset*/
    case 3 : Serial.println ("SW_RESET");break;               /**<3,  Software reset digital core*/
    case 4 : Serial.println ("OWDT_RESET");break;             /**<4,  Legacy watch dog reset digital core*/
    case 5 : Serial.println ("DEEPSLEEP_RESET");break;        /**<5,  Deep Sleep reset digital core*/
    case 6 : Serial.println ("SDIO_RESET");break;             /**<6,  Reset by SLC module, reset digital core*/
    case 7 : Serial.println ("TG0WDT_SYS_RESET");break;       /**<7,  Timer Group0 Watch dog reset digital core*/
    case 8 : Serial.println ("TG1WDT_SYS_RESET");break;       /**<8,  Timer Group1 Watch dog reset digital core*/
    case 9 : Serial.println ("RTCWDT_SYS_RESET");break;       /**<9,  RTC Watch dog Reset digital core*/
    case 10 : Serial.println ("INTRUSION_RESET");break;       /**<10, Instrusion tested to reset CPU*/
    case 11 : Serial.println ("TGWDT_CPU_RESET");break;       /**<11, Time Group reset CPU*/
    case 12 : Serial.println ("SW_CPU_RESET");break;          /**<12, Software reset CPU*/
    case 13 : Serial.println ("RTCWDT_CPU_RESET");break;      /**<13, RTC Watch dog Reset CPU*/
    case 14 : Serial.println ("EXT_CPU_RESET");break;         /**<14, for APP CPU, reseted by PRO CPU*/
    case 15 : Serial.println ("RTCWDT_BROWN_OUT_RESET");break;/**<15, Reset when the vdd voltage is not stable*/
    case 16 : Serial.println ("RTCWDT_RTC_RESET");break;      /**<16, RTC Watch dog reset digital core and rtc module*/
    default : Serial.println ("NO_MEAN");
  }
}

void print_time() {
time_t now;
char strftime_buf[64];
struct tm timeinfo;

time(&now);
setenv("TZ", "EST5EDT,M3.2.0,M11.1.0", 1);
tzset();

localtime_r(&now, &timeinfo);
strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
Serial.print("Time now is: ");
Serial.println(strftime_buf);
}
