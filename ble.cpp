#if USE_BLE

#include "ble.h"

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <Arduino.h>

#include <Preferences.h>
#include "display.h"


#define SERVICE_UUID        "726F8BB7-2002-4A36-8F10-ED6172B1D4E0"
#define CHARACTERISTIC_UUID "F2E0D699-B0FB-4F24-AAAD-A3D823532D71"

int has_connected_client = 0;

struct ns_data {
    uint16_t sgv;
    int8_t delta;
    uint32_t ts;
};

extern int sgv;
extern int sgv_delta;
extern long sgv_ts;
extern Preferences preferences;
extern long sleep_request_ms;


BLEServer *pServer = NULL;
uint16_t conn_id = 0;

int ble_get_battery_mv() {
  // read slowly to avoid noise
  analogSetClockDiv(255);
  int v = analogReadMilliVolts(35);
  // raw millivolts is half of actual battery voltage. multiply by two to get actual voltage
  v = v * 2;
  return v;
}

void got_new_data() {
    
    char buf[100];
      long prev_sgv_ts = preferences.getLong64("prev_sgv_ts", 0);
      if (sgv_ts != prev_sgv_ts ) {
        Serial.printf("Got new data from nightscout, or hard/soft reset; old ts=%ld new ts=%ld sgv=%d sgv_delta=%d\n", prev_sgv_ts, sgv_ts, sgv, sgv_delta);
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
        int battery_mv = ble_get_battery_mv();
        //fullscreen_message_subtitle(buf, tsbuf);
        char headbuf[100];
        sprintf(headbuf, "battery: %d", battery_mv);
        fullscreen_message_subtitle_header(buf, tsbuf, headbuf);
        preferences.putLong64("prev_sgv_ts", sgv_ts);
        Serial.println("Requesting 60 second deep sleep");
        Serial.flush();
        sleep_request_ms = 60 * 1000;
      } else {
        Serial.printf("No new data from nightscout, and not hard/soft reset; old ts=%ld new ts=%ld sgv=%d sgv_delta=%d\n", prev_sgv_ts, sgv_ts, sgv, sgv_delta);
        Serial.println("Will request 1 minute deep sleep");
        Serial.flush();
        sleep_request_ms = 60 * 1000;
      }
}

class MyCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    int data_size = sizeof(ns_data);
    if (pCharacteristic->getValue().length() != data_size) {
      Serial.printf("Invalid data size, %d expected, got %d\n", data_size, pCharacteristic->getValue().length());
      return;
    }
    
    struct ns_data *data = (struct ns_data *)pCharacteristic->getValue().data();
    Serial.printf("Received data: sgv=%d, delta=%d, ts=%u\n", data->sgv, data->delta, data->ts);
    sgv = data->sgv;
    sgv_delta = data->delta;
    sgv_ts = data->ts;
    got_new_data();
  }
};

class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer *server) {
    has_connected_client = 1;
    conn_id = server->getConnId();
  }

  void onDisconnect(BLEServer *server) {
    has_connected_client = 0;
    conn_id = 0;
  }

};

void ble_setup() {
    Serial.println("Starting BLE work!");

    BLEDevice::init("esp32c3-test");
    pServer = BLEDevice::createServer();
    BLEService *pService = pServer->createService(SERVICE_UUID);
    BLECharacteristic *pCharacteristic =
    pService->createCharacteristic(CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
    pCharacteristic->setCallbacks(new MyCallbacks());

    pCharacteristic->setValue("Hello World says Yakov");
    pService->start();
    // BLEAdvertising *pAdvertising = pServer->getAdvertising();  // this still is working for backward compatibility
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
    pAdvertising->setMinPreferred(0x12);
    BLEDevice::startAdvertising();
    Serial.println("Characteristic defined! Now you can read it in your phone!");

    pServer->setCallbacks(new MyServerCallbacks());
}

void ble_stop() {
  Serial.println("Stopping BLE work!");
  if (pServer) {
    Serial.print("Disconnecting conn_id=");
    Serial.println(conn_id);
    pServer->disconnect(conn_id);
    Serial.println("Disconnected.");
  }
    BLEDevice::deinit(true);
    Serial.println("Stopped BLE work");
}

#endif // USE_BLE