#ifndef CMMC_ConfigManager_H
#define CMMC_ConfigManager_H

#include <ArduinoJson.h>
//
// extern "C" {
// #include "user_interface.h"
// }
#include <SPIFFS.h>
#include "WiFi.h"
#include <functional>
#include <map>
#include "FS.h"

// #ifdef

#define CMMC_ConfigManager ConfigManager

typedef void (*cmmc_err_status_t)(uint8_t status, const char* cause);
typedef void (*cmmc_succ_status_t)(uint8_t status);
typedef void (*cmmc_debug_cb_t)(const char* cause);
typedef std::function<void(const char* msg, const char* k, const char* v)> cmmc_dump_cb_t;
typedef std::function<void(JsonObject* root, const char* content)> cmmc_json_loaded_cb_t;


#define USER_DEBUG_PRINTF(fmt, args...) { \
    Serial.printf(fmt, ## args); \
  }

class CMMC_ConfigManager
{
  public:
    // constructor
    CMMC_ConfigManager(const char* filename = "/config.json");
    ~CMMC_ConfigManager();
    typedef std::map<String, String> Items;
    void init(const char* filename = NULL);
    void commit();
    void load_config(cmmc_json_loaded_cb_t cb = NULL);
    void add_debug_listener(cmmc_debug_cb_t cb);
    void add_field(const char* key, const char* value);
    void dump_json_object(cmmc_dump_cb_t printer);
    char* fileContent = 0;
  private:
    char filename_c[60];
    bool _busy = false;
    File _init_json_file();
    Items items;
    DynamicJsonBuffer jsonBuffer;
    cmmc_debug_cb_t _user_debug_cb;
    File configFile;
    char _k[40] = {0};
    char _v[60] = {0};
    void _load_raw_content();
};
#endif //CMMC_ConfigManager_H
