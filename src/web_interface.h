#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <WiFi.h>

#include "display.h"

// const char *ssid = "GEmOPS";
// const char *password = "pleasebuy";

class WebInterface
{
public:
    WebInterface(char *ssid, char *password, std::vector<Display::Point> &points) : kSsid{ssid}, kPassword{password}
    {
        points_ = &points;
    }

    void Initialize()
    {
        WiFi.softAP(kSsid, kPassword);

        initSPIFFS();
        // initWiFi();
        initWebSocket();
        initWebServer();
    }

private:
    const char *kSsid;
    const char *kPassword;

    static std::vector<Display::Point> *points_;

    AsyncWebServer server{80};
    AsyncWebSocket ws{"/ws"};

    void initWebServer()
    {
        server.on("/", onRootRequest);
        server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");
        server.begin();
    }

    void initSPIFFS()
    {
        if (!SPIFFS.begin())
        {
            Serial.println("Cannot mount SPIFFS volume...");
            while (1)
            {
            }
        }
    }

    /* void initWiFi()
    {
        WiFi.mode(WIFI_STA);
        WiFi.begin(ssid, password);
        Serial.printf("Trying to connect [%s] ", WiFi.macAddress().c_str());
        while (WiFi.status() != WL_CONNECTED)
        {
            Serial.print(".");
            delay(500);
        }
        Serial.printf(" %s\n", WiFi.localIP().toString().c_str());
    } */

    static void onRootRequest(AsyncWebServerRequest *request) { request->send(SPIFFS, "/index.html", "text/html"); }

    void initWebSocket()
    {
        ws.onEvent(onEvent);
        server.addHandler(&ws);
    }

    static void onEvent(
        AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
    {
        switch (type)
        {
            case WS_EVT_CONNECT:
                Serial.printf(
                    "WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
                break;
            case WS_EVT_DISCONNECT:
                Serial.printf("WebSocket client #%u disconnected\n", client->id());
                break;
            case WS_EVT_DATA:
                handleWebSocketMessage(arg, data, len);
            case WS_EVT_PONG:
            case WS_EVT_ERROR:
                break;
        }
    }

    static void handleWebSocketMessage(void *arg, uint8_t *data, size_t len)
    {
        AwsFrameInfo *info = (AwsFrameInfo *)arg;
        if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT)
        {
            const uint8_t size = JSON_OBJECT_SIZE(10000);
            StaticJsonDocument<size> json;
            DeserializationError err = deserializeJson(json, data);
            if (err)
            {
                Serial.print(F("deserializeJson() failed with code "));
                Serial.println(err.c_str());
                return;
            }

            points_->clear();

            int num_points = json["num_points"];
            for (int i = 0; i < num_points; i++)
            {
                points_->push_back(Display::Point{json["points"][i]["x"], json["points"][i]["y"]});
            }
        }
    }

    void notifyClients()
    {
        // NOthing here unless we want to use it
        // ws.textAll(led.on ? "on" : "off");
    }
};
