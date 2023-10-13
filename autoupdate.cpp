#include "autoupdate.h"
#include <HTTPUpdate.h>
#include <WiFiClient.h>

const char* FW_URL_BASE = "http://192.168.1.33:54321/fw/";  // where *.bin and *.version files go


WiFiClient client;

void cb_start() {
  Serial.println("Update started");
}

void cb_progress(int cur, int total) {
  Serial.printf("Progress: %d%%\n", cur / (total / 100));
}

void cb_end() {
  Serial.println("\nUpdate complete");
}

void cb_error(int err) {
  Serial.printf("Error[%d]: ", err);
  if (err == HTTP_UPDATE_NO_UPDATES) {
    Serial.println("No updates");
  } else if (err == HTTP_UPDATE_OK) {
    Serial.println("OK");
  } else if (err == HTTP_UPDATE_FAILED) {
    Serial.println("Failed");
  } else if (err == HTTP_UPDATE_NO_UPDATES) {
    Serial.println("No updates");
  } else {
    Serial.println("Unknown");
  }
}

void check_for_remote_update(char* macAddr) {
  String myBaseURL = String(FW_URL_BASE);
  myBaseURL.concat(macAddr);
  String versionURL = myBaseURL;
  versionURL.concat(".version");

  HTTPClient httpClient;
  httpClient.begin(client, versionURL);
  Serial.print("Checking for new firmware version at ");
  Serial.println(versionURL);
  int httpCode = httpClient.GET();
  if (httpCode == 200) {
    String newFWVersion = httpClient.getString();
    Serial.print("Current FW Version: ");
    Serial.println(FW_VERSION);
    Serial.print("Newest available FW version: ");
    Serial.println(newFWVersion);
    int newFWVersionInt = newFWVersion.toInt();
    if (newFWVersionInt > FW_VERSION) {
      String newFWURL = myBaseURL;
      newFWURL.concat(".");
      newFWURL.concat(newFWVersion);
      newFWURL.concat(".bin");
      Serial.print("Preparing to update from ");
      Serial.println(newFWURL);
      httpUpdate.onStart(cb_start);
      httpUpdate.onError(cb_error);
      httpUpdate.onEnd(cb_end);
      httpUpdate.onProgress(cb_progress);
      t_httpUpdate_return ret = httpUpdate.update(client, newFWURL);
      switch(ret) {
        case HTTP_UPDATE_FAILED:
          Serial.printf("HTTP_UPDATE_FAILED: Error %d: %s\n",
              httpUpdate.getLastError(),
              httpUpdate.getLastErrorString().c_str());
          break;
        case HTTP_UPDATE_NO_UPDATES:
          Serial.println("HTTP_UPDATE_NO_UPDATES");
          break;
      }
    } else {
      Serial.println("Already on latest version.");
    }
  } else {
    Serial.print("Unable to check for new firmware version, got HTTP ");
    Serial.println(httpCode);
  }
  httpClient.end();
}
