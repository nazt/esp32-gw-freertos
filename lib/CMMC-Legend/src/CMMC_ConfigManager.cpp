#include "CMMC_ConfigManager.h"
#include "FS.h"

CMMC_ConfigManager::CMMC_ConfigManager(const char* filename) { 
  strcpy(this->filename_c, filename); 
  this->_user_debug_cb = [](const char* s) { };
}

CMMC_ConfigManager::~CMMC_ConfigManager() {
  configFile.close();
}

void CMMC_ConfigManager::_load_raw_content() {
  this->configFile = SPIFFS.open(this->filename_c, "r");
  size_t size = configFile.size() + 1;
  std::unique_ptr<char[]> buf(new char[size + 1]);
  bzero(buf.get(), size + 1);
  configFile.readBytes(buf.get(), size);
  if (this->fileContent != 0) {
    delete this->fileContent;
  }

  this->fileContent = new char[size + 1];
  strcpy(this->fileContent, buf.get());

  this->configFile.close();
}

void CMMC_ConfigManager::init(const char* filename) {
  if (filename != NULL) {
    strcpy(this->filename_c, filename); 
    // Serial.println("ConfigManager init with new filename");
  }
  else {
    // Serial.println("ConfigManager init()"); 
  }

  Serial.printf("current file = %s\r\n", this->filename_c);
  if (SPIFFS.exists(this->filename_c)) {
    // _load_raw_content();
  }
  else {
    USER_DEBUG_PRINTF("creating.. %s \r\n", this->filename_c);
    _init_json_file();
    _load_raw_content();
  }
}

void CMMC_ConfigManager::commit() {
  static CMMC_ConfigManager *that;
  that = this;
  USER_DEBUG_PRINTF("Commit FS..... from [%x]\r\n", that); 
  Serial.printf("> [BEFORE INNER CLOSURE] writing file %s\r\n", filename_c);
  load_config([](JsonObject * root, const char* content) {
    Serial.println("------------");
    Serial.printf("before commit DO: print config... from [%x]\r\n", root);
    Serial.println(content);
    Serial.println("------------");
    if (root != NULL) {
      Serial.printf("writing file %s\r\n", that->filename_c);
      that->configFile = SPIFFS.open(that->filename_c, "w");
      for (Items::iterator it = that->items.begin(); it != that->items.end(); ++it) {
        String first  = it->first;
        String second = it->second;
        root->set(first, second);
        Serial.printf("[std::map]: %s->%s\r\n", first.c_str(), second.c_str());
      }
      size_t configSize = root->printTo(that->configFile);
      root->printTo(Serial);
      Serial.printf(" has be written %d bytes to file.\r\n", that->configFile.size());
      that->configFile.close();
    }
    else {
      Serial.printf("loading config FAILED!!\r\n");
    }
  });
}

void CMMC_ConfigManager::add_field(const char* key, const char* value) {
  strcpy(this->_k, key);
  strcpy(this->_v, value);
  Serial.printf("> [ADD FILE AND SHOW FILENAME] writing file %s\r\n", filename_c);
  static CMMC_ConfigManager *that;
  that = this;
  USER_DEBUG_PRINTF("START [add_field] %s ----> %s (with addr: %x)\r\n", key, value, that);
  items[_k] = _v;
  // show content:
  // USER_DEBUG_PRINTF("Iterate through items object...\r\n");
  // for (Items::iterator it = items.begin(); it != items.end(); ++it) {
  //   USER_DEBUG_PRINTF("> %s->%s\r\n", it->first.c_str(), it->second.c_str());
  // }
}

void CMMC_ConfigManager::load_config(cmmc_json_loaded_cb_t cb) {
  _load_raw_content();
  jsonBuffer.clear(); 
  const char *b =  this->fileContent;
  JsonObject& json = jsonBuffer.parseObject(b);
  if (cb) {
    if (json.success()) {
      // USER_DEBUG_PRINTF("[load_config] Parsing config success.\r\n");
      cb(&json, this->fileContent);
    }
    else {
      // USER_DEBUG_PRINTF("[load_config] json parse failed.\r\n");
      cb(NULL, this->fileContent);
    }
  }
  else {
    Serial.println("CB = NULL");
  }
}

File CMMC_ConfigManager::_init_json_file() {
  USER_DEBUG_PRINTF("[_init_json_file]\r\n");
  this->configFile = SPIFFS.open(this->filename_c, "w");
  jsonBuffer.clear();
  JsonObject& json = this->jsonBuffer.createObject();
  json.printTo(configFile);
  this->configFile.close();
}

void CMMC_ConfigManager::add_debug_listener(cmmc_debug_cb_t cb) {
  if (cb != NULL) {
    this->_user_debug_cb = cb;
  }
}

void CMMC_ConfigManager::dump_json_object(cmmc_dump_cb_t printer) {
  // this->load_config();
  // if (this->currentJsonObject == NULL) {
  //   return;
  // }
  // else {
  //   this->currentJsonObject->printTo(Serial);
  //   char str_buffer[30];
  //   JsonObject* obj = this->currentJsonObject;
  //   for (JsonObject::iterator it = obj->begin(); it != obj->end(); ++it) {
  //     const char* key = it->key;
  //     const char* value = it->value;
  //     sprintf(str_buffer, "[key] %s -> %s\r\n", key, value);
  //     printer(str_buffer, key, value);
  //   }
  // }
}
