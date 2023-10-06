#include "display.h"
#include "autoupdate.h"
#include "nightscout.h"
#include "arduino_secrets.h"

int wifi_signal;
extern const char* FW_URL_BASE;

#include <Arduino.h>

#include <WiFiClient.h>
#include <HTTPClient.h>

#include <WiFi.h>
#include <WiFiManager.h>
WiFiManager wifiManager;



uint8_t mac[6];
char macAddr[14];

//const char HelloWorld[] = "Hello World!";
char* HelloWorld = "Starting up...";


int sgv;
int sgv_delta;
long sgv_ts;
long prev_sgv_ts;
boolean UpdateLocalTime();
#include "esp32/rom/rtc.h"

void print_reset_reason(int);
void print_time();
void set_date_from_server_date_header(const char* date_header);




void update_server_battery() {
  int v = analogRead(35);
  Serial.println("Updating server with battery level...");
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
void update_server_cache_version() {
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

void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());

  Serial.println(myWiFiManager->getConfigPortalSSID());
  char buf[100];
  display_init();
  HelloWorld = buf;
  sprintf(HelloWorld, "AP: %s", myWiFiManager->getConfigPortalSSID().c_str());
  helloWorld();
  display_hibernate();
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
  s2.concat(FW_VERSION);
  s2.concat(".bin");
  Serial.print("Place the firmware file e.g. here: ");
  Serial.println(s2);

  wifiManager.setAPCallback(configModeCallback);

  wifiManager.setConfigPortalTimeout(60);
  return wifiManager.autoConnect();

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
  delay(2000);

  Serial.println("CPU0 reset reason:");
  print_reset_reason(rtc_get_reset_reason(0));
  delay(2000);

  print_time();
  delay(1000);

  Serial.println("creating display");

    Serial.println("initting display");

  display_init();  
  Serial.println("hello world'ing");
  helloWorld();
  Serial.println("hibernating...");
  display_hibernate();
  if (init_wifi()) {
    char buf[100];
    sprintf(buf, "%s", macAddr);
    HelloWorld = buf;
    display_init();
    helloWorld();
    display_hibernate();
    delay(1000);
    update_server_cache_version();
      check_for_remote_update();
      if (update_nightscout()) {
        char buf[100];
        if (sgv_ts != prev_sgv_ts) {
          if (sgv_delta >= 0) 
            sprintf(buf, "%d +%d", sgv, sgv_delta);
          else
            sprintf(buf, "%d %d", sgv, sgv_delta);
          HelloWorld = buf;
  display_init();
          helloWorld();
display_hibernate();

xkcd_434();
esp_sleep_enable_timer_wakeup(60*1000000);
esp_deep_sleep_start();
        }
      } else {
delay(5000);
ESP.restart();
      }
    xkcd_434();
  } else {
    char buf[100];
    sprintf(buf, "Restarting...");
    HelloWorld = buf;
    helloWorld();
    delay(5000);
    ESP.restart();
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