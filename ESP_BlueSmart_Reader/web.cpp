/***************************************************************************
  Copyright (c) 2023 WASN.eu

  Licensed under the MIT License. You may not use this file except in
  compliance with the License. You may obtain a copy of the License at

  https://opensource.org/licenses/MIT

***************************************************************************/

#include "config.h"
#include "mqtt.h"
#include "utils.h"
#include "victron.h"
#include "web.h"
#include "nvs.h"
#include "wlan.h"


// local webserver on port 80 with OTA-Option
//AsyncWebServer httpServer(80);
ESP8266WebServer httpServer(80);

static char msgType[32];
static uint8_t msgTimeout;


// required for RESTful api
static void setCrossOrigin() {
    httpServer.sendHeader(F("Access-Control-Allow-Origin"), F("*"));
    httpServer.sendHeader(F("Access-Control-Max-Age"), F("600"));
    httpServer.sendHeader(F("Access-Control-Allow-Methods"), F("GET,OPTIONS"));
    httpServer.sendHeader(F("Access-Control-Allow-Headers"), F("*"));
}


// response for REST preflight request
static void sendCORS() {
    setCrossOrigin();
    httpServer.sendHeader(F("Access-Control-Allow-Credentials"), F("false"));
    httpServer.send(204);
}


// passes updated value to web ui as JSON on AJAX call once a second
// can also be used for (remote) RESTful request
static void handleGetReadings() {
    StaticJsonDocument<384> JSON;
    static char reply[288];

    JSON.clear();
    JSON["voltage"] = victron.voltage;
    JSON["current"] = victron.current;
    JSON["runtime"] = getRuntime(false);
    JSON["rssi"] = WiFi.RSSI();

    // only relevant for power meter's web ui
    if (httpServer.arg("local").length() >= 1) {
        if (strlen(msgType) > 0) {
            JSON["msgType"] = msgType;
            JSON["msgTimeout"] = msgTimeout;
            memset(msgType, 0, sizeof(msgType));
            msgTimeout = 0;
        }
#ifdef DEBUG_HEAP
        JSON["freeheap"] = ESP.getFreeHeap();
#endif
    }
    memset(reply, 0, sizeof(reply));
    size_t s = serializeJson(JSON, reply);
    setCrossOrigin(); // required for remote REST queries
    httpServer.send(200, "text/plain", reply, s);
}


// display message for given time in web ui
void setMessage(const char *msg, uint8_t timeSecs) {
    strlcpy(msgType, msg, sizeof(msgType));
    msgTimeout = timeSecs;
}


// terminate webserver (power saving mode)
void stopWebserver() {
    httpServer.stop();
    Serial.println(F("Webserver stopped"));
}


// handle and dispatch http request
void handleWebrequest() {
    httpServer.handleClient();
}

void sendFavicon() {

}

// configure url handler and start web server
void startWebserver() {

    // send main page
    httpServer.on("/", HTTP_GET, []() {
        String html = FPSTR(HEADER_html);
        html += FPSTR(MAIN_html);
        html += FPSTR(FOOTER_html);
        html.replace("__SYSTEMID__", systemID());
        html.replace("__FIRMWARE__", String(FIRMWARE_VERSION));
        html.replace("__BUILD__", String(__DATE__)+" "+String(__TIME__));
        html.replace("__HOSTNAME__", WiFi.hostname());
        html.replace("__LOCALIP__", WiFi.localIP().toString());
        Serial.println(F("Show main page"));
        httpServer.send(200, "text/html", html);
    });    
    
    httpServer.on("/index.css", HTTP_GET,  []() { httpServer.send(200, "text/css", indexcss);} );
    //httpServer.on("/favicon.ico", HTTP_GET, []() { httpServer.send(200, "image/x-icon", faviconico);} );
    //httpServer.on("/favicon.ico", HTTP_GET, sendFavicon);

    // handler for AJAX requests
    httpServer.on("/readings", HTTP_GET, handleGetReadings);
    httpServer.on("/readings", HTTP_OPTIONS, sendCORS);

    // restart ESP8266
    httpServer.on("/restart", HTTP_GET, []() {
        httpServer.send(200, "text/plain", "OK", 2);
        restartSystem();
    });

    // show upload form for firmware update
    httpServer.on("/update", HTTP_GET, []() {
        String html = FPSTR(HEADER_html);
        if (httpServer.arg("res") == "ok") {
            html += FPSTR(UPDATE_OK_html);
            Serial.println(F("Show firmware upload success"));
        } else if (httpServer.arg("res") == "err") {
            html += FPSTR(UPDATE_ERR_html);
            Serial.println(F("Show firmware upload failed"));
        } else {
            html += FPSTR(UPDATE_html);
            Serial.println(F("Show firmware upload form"));
        }
        html += FPSTR(FOOTER_html);
        html.replace("__SYSTEMID__", systemID());
        html.replace("__FIRMWARE__", String(FIRMWARE_VERSION));
        html.replace("__BUILD__", String(__DATE__)+" "+String(__TIME__));
        html.replace("__HOSTNAME__", WiFi.hostname());
        html.replace("__LOCALIP__", WiFi.localIP().toString());
        httpServer.send(200, "text/html", html);
    });  

    // show main configuration
    httpServer.on("/config", HTTP_GET, []() {
        String html = FPSTR(HEADER_html);
        html += FPSTR(CONFIG_html);
        html += FPSTR(FOOTER_html);
        
        html.replace("__VOLTAGE__", String(victron.voltage));
        html.replace("__CURRENT", String(victron.current));
        html.replace("__BACKUP_CYCLE__", String(settings.backupCycleMin));
        html.replace("__BACKUP_CYCLE_MIN__", String(BACKUP_CYCLE_MIN));
        html.replace("__BACKUP_CYCLE_MAX__", String(BACKUP_CYCLE_MAX));
        html.replace("__METER_ID__", systemID());
        html.replace("__HOSTNAME__", WiFi.hostname());
        html.replace("__LOCALIP__", WiFi.localIP().toString());
        
        if (settings.enableMQTT)
            html.replace("__MQTT__", "checked");
        else
            html.replace("__MQTT__", "");
        html.replace("__MQTT_BROKER__", String(settings.mqttBroker));
        html.replace("__MQTT_PORT__", String(settings.mqttBrokerPort));
        html.replace("__MQTT_BASE_TOPIC__", String(settings.mqttBaseTopic));
        if (settings.mqttJSON)
            html.replace("__MQTT_JSON__", "checked");
        else
            html.replace("__MQTT_JSON__", "");
        if (settings.enableHADiscovery)
            html.replace("__MQTT_HA_DISCOVERY__", "checked");
        else
            html.replace("__MQTT_HA_DISCOVERY__", "");
        html.replace("__MQTT_INTERVAL__", String(settings.mqttIntervalSecs));
        if (settings.mqttEnableAuth) 
            html.replace("__MQTT_AUTH__", "checked");
        else
            html.replace("__MQTT_AUTH__", "");        
        html.replace("__MQTT_USERNAME__", String(settings.mqttUsername));
        html.replace("__MQTT_PASSWORD__", String(settings.mqttPassword));
        if (settings.mqttSecure)
            html.replace("__MQTT_SECURE__", "checked");
        else
            html.replace("__MQTT_SECURE__", "");
        if (settings.enablePowerSavingMode) {
            html.replace("__POWER_SAVING_MODE__", "checked");
        } else {
            html.replace("__POWER_SAVING_MODE__", "");
        }
        html.replace("__MQTT_INTERVAL_MIN_POWERSAVING__", String(MQTT_INTERVAL_MIN_POWERSAVING));

        html.replace("__SYSTEMID__", systemID());
        html.replace("__FIRMWARE__", String(FIRMWARE_VERSION));
        html.replace("__BUILD__", String(__DATE__)+" "+String(__TIME__));
        Serial.println(F("Show main configuration"));
        httpServer.send(200, "text/html", html);
    });   

    // save general settings
    httpServer.on("/config", HTTP_POST, []() {
        uint16_t mqttIntervalMinSecs;
        if (httpServer.arg("meter_id").length() >= METER_ID_LEN_MIN && httpServer.arg("meter_id").length() <= 16)
            strlcpy(settings.systemID, httpServer.arg("meter_id").c_str(), 16);
        else
            memset(settings.systemID, 0, sizeof(settings.systemID));
        if (httpServer.arg("backup_cycle").toInt() >= BACKUP_CYCLE_MIN &&
                httpServer.arg("backup_cycle").toInt() <= BACKUP_CYCLE_MAX)
            settings.backupCycleMin = httpServer.arg("backup_cycle").toInt();  

        if (httpServer.arg("mqtt") == "on") {
            settings.enableMQTT = true;
            if (httpServer.arg("powersavingmode") == "on") {
                settings.enablePowerSavingMode = true;
                mqttIntervalMinSecs = MQTT_INTERVAL_MIN_POWERSAVING; // to actually save power...
            } else {
                settings.enablePowerSavingMode = false;
                mqttIntervalMinSecs = MQTT_INTERVAL_MIN;
            }
            if (httpServer.arg("mqttbroker").length() >= MQTT_BROKER_LEN_MIN &&
                    httpServer.arg("mqttbroker").length() <= 64)
                strlcpy(settings.mqttBroker, httpServer.arg("mqttbroker").c_str(), 64);
            if (httpServer.arg("mqttport").toInt() >= MQTT_BROKER_PORT_MIN &&
                    httpServer.arg("mqttport").toInt() <= MQTT_BROKER_PORT_MAX)
                settings.mqttBrokerPort = httpServer.arg("mqttport").toInt();
            if (httpServer.arg("mqttbasetopic").length() >= MQTT_BASETOPIC_LEN_MIN &&
                    httpServer.arg("mqttbasetopic").length() <= 64)
                strlcpy(settings.mqttBaseTopic, httpServer.arg("mqttbasetopic").c_str(), 64);
            if (httpServer.arg("mqttinterval").toInt() >= mqttIntervalMinSecs &&
                    httpServer.arg("mqttinterval").toInt() <= MQTT_INTERVAL_MAX) {
                if (httpServer.arg("mqttinterval").toInt() != settings.mqttIntervalSecs)
                settings.mqttIntervalSecs = httpServer.arg("mqttinterval").toInt();
            } else if (httpServer.arg("mqttinterval").toInt() < mqttIntervalMinSecs)
                settings.mqttIntervalSecs = mqttIntervalMinSecs;
            else if (httpServer.arg("mqttinterval").toInt() > MQTT_INTERVAL_MAX)
                settings.mqttIntervalSecs = MQTT_INTERVAL_MAX;
            if (httpServer.arg("mqtt_json") == "on")
                settings.mqttJSON = true;
            else
                settings.mqttJSON = false;
            if (httpServer.arg("mqtt_ha_discovery") == "on")
                settings.enableHADiscovery = true;
            else
                settings.enableHADiscovery = false;
            if (httpServer.arg("mqttauth") == "on") {
                settings.mqttEnableAuth = true;
                if (httpServer.arg("mqttuser").length() >= MQTT_USER_LEN_MIN &&
                        httpServer.arg("mqttuser").length() <= 32)
                    strlcpy(settings.mqttUsername, httpServer.arg("mqttuser").c_str(), 32);
                if (httpServer.arg("mqttpassword").length() >= MQTT_PASS_LEN_MIN &&
                        httpServer.arg("mqttpassword").length() <= 32)
                    strlcpy(settings.mqttPassword, httpServer.arg("mqttpassword").c_str(), 32);
            } else {
                settings.mqttEnableAuth = false;
            }
            if (httpServer.arg("mqtt_secure") == "on")
                settings.mqttSecure = true;
            else
                settings.mqttSecure = false;
        } else {
            settings.enableMQTT = false;
            settings.enablePowerSavingMode = false;
        }

        mqttDisconnect(false);
        saveNVS(true);
        mqttPublish();
        httpServer.sendHeader("Location", "/config?saved", true);
        httpServer.send(302, "text/plain", "");
        Serial.println(F("Updated main configuration"));
    });

    // show expert settings page
    httpServer.on("/expert", HTTP_GET, []() {
        String html = FPSTR(HEADER_html);
        html += FPSTR(EXPERT_html);
        html += FPSTR(FOOTER_html);

        html.replace("__VOLTAGE__", String(victron.voltage));
        html.replace("__CURRENT__", String(victron.current));
        html.replace("__HOSTNAME__", WiFi.hostname());
        html.replace("__LOCALIP__", WiFi.localIP().toString());
        if (settings.enableInflux) 
            html.replace("__INFLUXDB__", "checked");
        else
            html.replace("__INFLUXDB__", "");

        html.replace("__SYSTEMID__", systemID());
        html.replace("__FIRMWARE__", String(FIRMWARE_VERSION));
        html.replace("__BUILD__", String(__DATE__)+" "+String(__TIME__));
        Serial.println(F("Show expert settings"));
        httpServer.send(200, "text/html", html);
    });

    // save general settings
    httpServer.on("/expert", HTTP_POST, []() {
        if (httpServer.arg("influxdb") == "on")
            settings.enableInflux = true;
        else
            settings.enableInflux = false;

        saveNVS(true);
        httpServer.sendHeader("Location", "/expert?saved", true);
        httpServer.send(302, "text/plain", "");
        Serial.println(F("Update expert configuration"));
    });

    // handle firmware upload
    httpServer.on("/update", HTTP_POST, []() {
        if (Update.hasError()) {
            Serial.println(F("OTA failed"));
            httpServer.send(500, "text/plain", "ERROR");
            blinkLED(4, 50);
        } else {
            Serial.println(F("OTA successful"));
            httpServer.send(200, "text/plain", "OK");
            blinkLED(2, 250);
        }
    }, []() {
        HTTPUpload& upload = httpServer.upload();
        if (upload.status == UPLOAD_FILE_START) {
            saveNVS(false);
            Serial.println(F("Starting OTA update..."));
            uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
            if (!Update.begin(maxSketchSpace)) {
                Update.printError(Serial);
            }
        } else if (upload.status == UPLOAD_FILE_WRITE) {
            if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
                Update.printError(Serial);
                saveNVS(true);
            }
        } else if (upload.status == UPLOAD_FILE_END) {
            if (!Update.end(true)) {
                Update.printError(Serial);
                saveNVS(true);
            }
        }
    });

    // send configuration as JSON file
    httpServer.on("/nvsbackup", HTTP_GET, []() {
        String configfile;
        const char* configJSON;

        configJSON = nvs2json();
        if (configJSON != NULL) {
            Serial.printf("Sending configuration data as JSON (%d bytes)...\n", strlen(configJSON));
            configfile = "Victron_BlueSmart_" + systemID() + "_v" + String(FIRMWARE_VERSION) + ".json";
            httpServer.sendHeader("Content-Type", "text/plain");
            httpServer.sendHeader("Content-Disposition", "attachment; filename="+configfile);
            httpServer.setContentLength(strlen(configJSON));
            httpServer.sendHeader("Connection", "close");
            httpServer.send(200, "application/octet-stream", configJSON);
        } else {
            Serial.println(F("Failed to export configuration data!"));
            httpServer.send(500, "text/plain", "ERROR");
        }
    });


    // show upload form for firmware update
    httpServer.on("/nvsimport", HTTP_GET, []() {
        String html = FPSTR(HEADER_html);
        if (httpServer.arg("res") == "ok") {
            html += FPSTR(IMPORT_OK_html);
            Serial.println(F("Show configuration import success"));
        } else if (httpServer.arg("res") == "err") {
            html += FPSTR(IMPORT_ERR_html);
            Serial.println(F("Show configuration import failed"));
        } else {
            html += FPSTR(IMPORT_html);
            Serial.println(F("Show configuration import form"));
        }
        html += FPSTR(FOOTER_html);
        html.replace("__SYSTEMID__", systemID());
        html.replace("__FIRMWARE__", String(FIRMWARE_VERSION));
        html.replace("__BUILD__", String(__DATE__)+" "+String(__TIME__));
        html.replace("__HOSTNAME__", WiFi.hostname());
        html.replace("__LOCALIP__", WiFi.localIP().toString());
        httpServer.send(200, "text/html", html);
    });


    httpServer.on("/nvsimport", HTTP_POST, []() {
        HTTPUpload& upload = httpServer.upload();
        if (upload.status == UPLOAD_FILE_START) {
            Serial.println(F("Importing configuration data..."));
        } else if (upload.status == UPLOAD_FILE_END) {
            if (!json2nvs((const char*)upload.buf, upload.currentSize))
                httpServer.send(500, "text/plain", "ERROR");
            else
                httpServer.send(200, "text/plain", "OK");
        } else {
            Serial.println(F("Failed to import settings, upload error!"));
            httpServer.send(500, "text/plain", "ERROR");
        }
    });

    httpServer.begin();
    Serial.println(F("Webserver started"));
}
