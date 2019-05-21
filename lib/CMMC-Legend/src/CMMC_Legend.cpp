#include "CMMC_Legend.h"

void CMMC_Legend::addModule(CMMC_Module* module) {
  _modules.push_back(module);
  Serial.printf("addModule.. size = %d\r\n", _modules.size());
}

// being called by os
void CMMC_Legend::run() {
  static CMMC_Legend *that = this;
  int size = _modules.size();
  for (int i = 0 ; i < size; i++) {
    _modules[i]->loop();
  }
  isLongPressed();
  yield();
}

bool CMMC_Legend::setEnable(bool status) {
  if (status) {
    File f = SPIFFS.open("/enabled", "a+");
    return f;
  }
  else {
      return SPIFFS.remove("/enabled");
  }
}

void CMMC_Legend::isLongPressed() {
  uint32_t prev = millis();
  while (digitalRead(15) == HIGH) {
    delay(50);
    Serial.println("...isLongPressed..");
    if ( (millis() - prev) > 5L * 1000L) {
      Serial.println("LONG PRESSED.");
      blinker->blink(50);
      while (digitalRead(15) == HIGH) {
        delay(10);
      }
      setEnable(false);
      Serial.println("being restarted.");
      delay(1000);
      ESP.restart();
    }
  }
}

void CMMC_Legend::setup(os_config_t *config) {
  // CMMC_System::setup();
    Serial.begin(config->baudrate);
    init_gpio();
    init_fs();
    init_user_config();
    init_user_sensor();
    init_network();
}

void CMMC_Legend::init_gpio() {
  Serial.println("OS::Init GPIO..");
  pinMode(15, INPUT);
  blinker = new xCMMC_LED;
  blinker->init();
  blinker->setPin(16);
  Serial.println();
  blinker->blink(500);
  delay(10);
}

#include <FS.h> //this needs to be first, or it all crashes and burns...
#include "SPIFFS.h"
void CMMC_Legend::init_fs() {
  // Serial.println("OS::Init FS..");
  SPIFFS.begin();
  // Dir dir = SPIFFS.openDir("/");
  // isLongPressed();
  // Serial.println("--------------------------");
  // while (dir.next()) {
  //   File f = dir.openFile("r");
  //   Serial.printf("> %s \r\n", dir.fileName().c_str());
  // }
  /*******************************************
     Boot Mode Selection
   *******************************************/
  // Serial.println("--------------------------");
  if (!SPIFFS.exists("/enabled")) {
    mode = SETUP;
  }
  else {
    mode = RUN;
  }
}

void CMMC_Legend::init_user_sensor() {
  Serial.printf("Initializing Sensor.. MODE=%s\r\n", mode == SETUP ? "SETUP" : "RUN");
  if (mode == SETUP) {
    return;
  }
}

void CMMC_Legend::init_user_config() {
  Serial.println("init_user_config");
}

void CMMC_Legend::init_network() {
  Serial.println("Initializing network.");

  for (int i = 0 ; i < _modules.size(); i++) {
    Serial.println("call module.config()");
    _modules[i]->config(this, &server);
  }

  if (mode == SETUP) {
    Serial.println("calling confgSetup");
    for (int i = 0 ; i < _modules.size(); i++) {
      _modules[i]->configSetup();
    }

    _init_ap();

    setupWebServer(&server, &ws, &events);

    blinker->blink(50);
    uint32_t startConfigLoopAtMs = millis();
    while (1 && !stopFlag) {
      for (int i = 0 ; i < _modules.size(); i++) {
        _modules[i]->configLoop();
        yield();
      }
      if ( (millis() - startConfigLoopAtMs) > 20L*60*1000) {
          setEnable(true);
          delay(100);
          ESP.restart();
      }
    }

    SPIFFS.begin();
    File f = SPIFFS.open("/enabled", "a+");
    blinker->blink(50);
    delay(200);;
    ESP.restart();
  }
  else if (mode == RUN) {
    // system_update_cpu_freq(80);
    blinker->blink(4000);
    for (int i = 0 ; i < _modules.size(); i++) {
      _modules[i]->setup();
    }
  }
}

xCMMC_LED *CMMC_Legend::getBlinker() {
  return blinker;
}

void CMMC_Legend::_init_ap() {
  WiFi.disconnect();
  WiFi.softAPdisconnect();
  delay(100);
  WiFi.mode(WIFI_AP_STA);
  delay(100);
  IPAddress Ip(192, 168, 4, 1);
  IPAddress NMask(255, 255, 255, 0);
  WiFi.softAPConfig(Ip, Ip, NMask);
  sprintf(&this->ap_ssid[5], "%08x", "ffffff");
  WiFi.softAP(ap_ssid, &ap_ssid[5]);
  delay(20);
  IPAddress myIP = WiFi.softAPIP();
  Serial.println();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  delay(100);
}
void CMMC_Legend::setupWebServer(AsyncWebServer *server, AsyncWebSocket *ws, AsyncEventSource *events) {
  // ws->onEvent(this->onWsEvent);
  static CMMC_Legend *that;
  that = this;
  server->addHandler(ws);
  server->serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");
  events->onConnect([](AsyncEventSourceClient * client) {
    client->send("hello!", NULL, millis(), 1000);
  });
  server->addHandler(events);
  // server->addHandler(new SPIFFSEditor(http_username, http_password));

  server->on("/heap", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(200, "text/plain", String(ESP.getFreeHeap()));
  });

  server->on("/reboot", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(200, "text/plain", "OK");
    blinker->blink(20);
    ESP.restart();
  });

  server->on("/enable", HTTP_GET, [](AsyncWebServerRequest * request) {
    File f = SPIFFS.open("/enabled", "a+");
    if (!f) {
      Serial.println("file open failed");
    }
    request->send(200, "text/plain", String("ENABLING.. ") + String(ESP.getFreeHeap()));
    delay(1000);
    ESP.restart();
  });

  static const char* fsServerIndex = "<form method='POST' action='/do-fs' enctype='multipart/form-data'><input type='file' name='update'><input type='submit' value='Update'></form>";
  static const char* serverIndex = "<form method='POST' action='/do-firmware' enctype='multipart/form-data'><input type='file' name='update'><input type='submit' value='Update'></form>";
  server->on("/firmware", HTTP_GET, [](AsyncWebServerRequest * request) {
    AsyncWebServerResponse *response = request->beginResponse(200, "text/html", serverIndex);
    response->addHeader("Connection", "close");
    response->addHeader("Access-Control-Allow-Origin", "*");
    request->send(response);
  });

  server->on("/fs", HTTP_GET, [](AsyncWebServerRequest * request) {
    AsyncWebServerResponse *response = request->beginResponse(200, "text/html", fsServerIndex);
    response->addHeader("Connection", "close");
    response->addHeader("Access-Control-Allow-Origin", "*");
    request->send(response);
  });

  server->on("/do-fs", HTTP_POST, [](AsyncWebServerRequest * request) {
    // the request handler is triggered after the upload has finished...
    // create the response, add header, and send response
    bool updateHasError = Update.hasError();
    AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", (updateHasError) ? "FAIL" : "OK");
    response->addHeader("Connection", "close");
    response->addHeader("Access-Control-Allow-Origin", "*");
    // restartRequired = true;  // Tell the main loop to restart the ESP
    request->send(response);
    if (!updateHasError) {
      // delay(1000);
      // ESP.restart();
    }
  }, [](AsyncWebServerRequest * request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
    //Upload handler chunks in data
    if (!index) { // if index == 0 then this is the first frame of data
      SPIFFS.end();
      blinker->detach();
      Serial.println("upload start...");
      Serial.printf("UploadStart: %s\n", filename.c_str());
      Serial.setDebugOutput(true);
      // calculate sketch space required for the update
      uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
      bool updateOK = maxSketchSpace < ESP.getFreeSketchSpace();
      if (!Update.begin(maxSketchSpace, U_SPIFFS)) { //start with max available size
        Update.printError(Serial);
      }
      //TODO: ESP32
      // Update.runAsync(true); // tell the updaterClass to run in async mode
    }

    //Write chunked data to the free sketch space
    if (Update.write(data, len) != len) {
      Update.printError(Serial);
    }

    if (final) { // if the final flag is set then this is the last frame of data
      if (Update.end(true)) { //true to set the size to the current progress
        Serial.printf("Update Success: %u B\nRebooting...\n", index + len);
        blinker->blink(1000);
      } else {
        Update.printError(Serial);
      }
      Serial.setDebugOutput(false);
    }
  });

  server->on("/update", HTTP_POST, [&](AsyncWebServerRequest *request){
    // the request handler is triggered after the upload has finished...
    // create the response, add header, and send response
    AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", (Update.hasError())?"FAIL":"OK");
    response->addHeader("Connection", "close");
    response->addHeader("Access-Control-Allow-Origin", "*");
    request->send(response);
  },[&](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final){
    //Upload handler chunks in data
    if(!index){ // if index == 0 then this is the first frame of data
      Serial.printf("UploadStart: %s\n", filename.c_str());
      Serial.setDebugOutput(true);

      // calculate sketch space required for the update
      uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
      if(!Update.begin(maxSketchSpace)){//start with max available size
        Update.printError(Serial);
      }
      // TODO: ESP32
      // Update.runAsync(true); // tell the updaterClass to run in async mode
    }

    //Write chunked data to the free sketch space
    if(Update.write(data, len) != len){
        Update.printError(Serial);
    }

    if(final){ // if the final flag is set then this is the last frame of data
      if(Update.end(true)){ //true to set the size to the current progress
          Serial.printf("Update Success: %u B\nRebooting...\n", index+len);
          that->stopFlag = true;
          stopFlag = true;
        } else {
          Update.printError(Serial);
        }
        Serial.setDebugOutput(false);
    }
  });


  server->on("/do-firmware", HTTP_POST, [](AsyncWebServerRequest * request) {
    // the request handler is triggered after the upload has finished...
    // create the response, add header, and send response
    bool updateHasError = Update.hasError();
    AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", (updateHasError) ? "FAIL" : "OK");
    response->addHeader("Connection", "close");
    response->addHeader("Access-Control-Allow-Origin", "*");
    // restartRequired = true;  // Tell the main loop to restart the ESP
    request->send(response);
    if (!updateHasError) {
      // delay(1000);
      // ESP.restart();
    }
  }, [&](AsyncWebServerRequest * request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
    //Upload handler chunks in data
    if (!index) { // if index == 0 then this is the first frame of data
      SPIFFS.end();
      blinker->detach();
      Serial.println("upload start...");
      Serial.printf("UploadStart: %s\n", filename.c_str());
      Serial.setDebugOutput(true);
      // calculate sketch space required for the update
      uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
      bool updateOK = maxSketchSpace < ESP.getFreeSketchSpace();
      if (!Update.begin(maxSketchSpace, U_FLASH)) { //start with max available size
        Update.printError(Serial);
      }
      // TODO: ESP32
      // Update.runAsync(true); // tell the updaterClass to run in async mode
    }

    //Write chunked data to the free sketch space
    if (Update.write(data, len) != len) {
      Update.printError(Serial);
    }

    if (final) { // if the final flag is set then this is the last frame of data
      if (Update.end(true)) { //true to set the size to the current progress
        Serial.printf("Update Success: %u B\nRebooting...\n", index + len);
          that->stopFlag = true;
          stopFlag = true;
        blinker->blink(1000);
      } else {
        Update.printError(Serial);
      }
      Serial.setDebugOutput(false);
    }
  });

  server->onNotFound([](AsyncWebServerRequest * request) {
    Serial.printf("NOT_FOUND: ");
    if (request->method() == HTTP_GET)
      Serial.printf("GET");
    else if (request->method() == HTTP_POST)
      Serial.printf("POST");
    else if (request->method() == HTTP_DELETE)
      Serial.printf("DELETE");
    else if (request->method() == HTTP_PUT)
      Serial.printf("PUT");
    else if (request->method() == HTTP_PATCH)
      Serial.printf("PATCH");
    else if (request->method() == HTTP_HEAD)
      Serial.printf("HEAD");
    else if (request->method() == HTTP_OPTIONS)
      Serial.printf("OPTIONS");
    else
      Serial.printf("UNKNOWN");
    Serial.printf(" http://%s%s\n", request->host().c_str(), request->url().c_str());

    if (request->contentLength()) {
      Serial.printf("_CONTENT_TYPE: %s\n", request->contentType().c_str());
      Serial.printf("_CONTENT_LENGTH: %u\n", request->contentLength());
    }

    int headers = request->headers();
    int i;
    // for (i = 0; i < headers; i++) {
    //   // AsyncWebHeader* h = request->getHeader(i);
    //   // Serial.printf("_HEADER[%s]: %s\n", h->name().c_str(), h->value().c_str());
    // }

    int params = request->params();
    for (i = 0; i < params; i++) {
      AsyncWebParameter* p = request->getParam(i);
      if (p->isFile()) {
        Serial.printf("_FILE[%s]: %s, size: %u\n", p->name().c_str(), p->value().c_str(), p->size());
      } else if (p->isPost()) {
        Serial.printf("_POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
      } else {
        Serial.printf("_GET[%s]: %s\n", p->name().c_str(), p->value().c_str());
      }
    }

    request->send(404);
  });

  server->begin();
  Serial.println("Starting webserver->..");
}
