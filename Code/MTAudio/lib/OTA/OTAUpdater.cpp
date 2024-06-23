#include "OTAUpdater.h"
#include <Arduino.h>

#define SSID_FORMAT "MT WebUpdate" 
#define PASSWORD "0123456789"
#define MDNS_NAME "mt-audio"

OTAUpdater::OTAUpdater()
{
    server = new WebServer(80);

    apMode();
    webServerInit();
}

OTAUpdater::~OTAUpdater()
{
    delete server;
}

void OTAUpdater::loop()
{
    delay(50);
    server->handleClient();
}

void OTAUpdater::apMode() {
  char ssid[13];
  char passwd[11];
  long unsigned int espmac = ESP.getEfuseMac() >> 24;
  snprintf(ssid, 13, SSID_FORMAT, espmac);
  snprintf(passwd, 11, PASSWORD);
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, passwd);  // Set up the SoftAP
  MDNS.begin(MDNS_NAME);
  Serial.printf("AP: %s, PASS: %s\n", ssid, passwd);
}

void OTAUpdater::handleUpdateEnd() {
  server->sendHeader("Connection", "close");
  if (Update.hasError()) {
    server->send(502, "text/plain", Update.errorString());
  } else {
    server->sendHeader("Refresh", "10");
    server->sendHeader("Location", "/");
    server->send(307);
    ESP.restart();
  }
}

void OTAUpdater::handleUpdate() {
  size_t fsize = UPDATE_SIZE_UNKNOWN;
  if (server->hasArg("size")) {
    fsize = server->arg("size").toInt();
  }
  HTTPUpload &upload = server->upload();
  if (upload.status == UPLOAD_FILE_START) {
    Serial.printf("Receiving Update: %s, Size: %d\n", upload.filename.c_str(), fsize);
    if (!Update.begin(fsize)) {
      otaDone = 0;
      Update.printError(Serial);
    }
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
      Update.printError(Serial);
    } else {
      otaDone = 100 * Update.progress() / Update.size();
    }
  } else if (upload.status == UPLOAD_FILE_END) {
    if (Update.end(true)) {
      Serial.printf("Update Success: %u bytes\nRebooting...\n", upload.totalSize);
    } else {
      Serial.printf("%s\n", Update.errorString());
      otaDone = 0;
    }
  }
}

void OTAUpdater::webServerInit() {
  server->on(
    "/update", HTTP_POST,
    [this]() {
      handleUpdateEnd();
    },
    [this]() {
      handleUpdate();
    }
  );
  server->onNotFound([this]() {
    server->send(200, "text/html", indexHtml);
  });
  server->begin();
}