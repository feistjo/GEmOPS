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
    WebInterface(char *ssid, char *password, Display &display /* std::vector<Display::Point> &points */)
        : kSsid{ssid}, kPassword{password}
    {
        /* points_ = &points; */
        display_ = &display;
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

    // static std::vector<Display::Point> *points_;
    static Display *display_;

    AsyncWebServer server{80};
    AsyncWebSocket ws{"/ws"};

    static std::vector<char> aws_data;

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
        // Serial.println("Handle ws message");
        AwsFrameInfo *info = (AwsFrameInfo *)arg;
        /* Serial.printf("Final: %d, index: %d, len: %d, len: %d, opcode: %d\n",
                      info->final,
                      info->index,
                      info->len,
                      len,
                      info->opcode == WS_TEXT);
        Serial.println(info->opcode); */
        Serial.println("Handle");
        Serial.printf("%s\n", data);
        if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT)
        {
            aws_data.clear();
            Serial.println("Received single ws message");

            const uint16_t size = JSON_OBJECT_SIZE(512);
            StaticJsonDocument<size> json;
            // DynamicJsonDocument json{10000};
            DeserializationError err = deserializeJson(json, data);
            if (err)
            {
                Serial.print(F("deserializeJson() failed with code "));
                Serial.println(err.c_str());
                return;
            }

            std::vector<Display::Point> *points = new std::vector<Display::Point>;
            // points_->clear();

            int num_points = json["num_points"];
            Serial.printf("Received %d points\n", num_points);
            for (int i = 0; i < num_points; i++)
            {
                // Serial.printf("Point %d: %f, %f\n", i, json[String(i)]["x"], json[String(i)]["y"]);
                float x = json[String(i)]["x"];
                float y = json[String(i)]["y"];
                Serial.printf("%s\n", json[String(i)]);
                Serial.printf("Point %d: %f, %f\n", i, x, y);
                points->push_back(Display::Point{x, y});
            }
            display_->SetImage(points);
            Serial.println("Updated image");
        }
        else if (!info->final || len + aws_data.size() != info->len)
        {
            Serial.println("Not final");
            Serial.println("Info len, size, len, index");
            Serial.println(info->len);
            Serial.println(aws_data.size());
            Serial.println(len);
            Serial.println(info->index);
            for (size_t i = 0; i < len; i++)
            {
                aws_data.push_back(data[i]);
            }
        }
        else
        {
            Serial.println("Final");
            for (size_t i = 0; i < len; i++)
            {
                aws_data.push_back(data[i]);
            }
            uint8_t full_data[aws_data.size()];
            for (size_t i = 0; i < aws_data.size(); i++)
            {
                full_data[i] = aws_data[i];
            }
            // Serial.printf("%s\n", full_data);

            const uint16_t size = JSON_OBJECT_SIZE(512);
            StaticJsonDocument<size> json;

            // DynamicJsonDocument json{10000};
            DeserializationError err = deserializeJson(json, full_data);
            if (err)
            {
                Serial.print(F("deserializeJson() failed with code "));
                Serial.println(err.c_str());
                return;
            }

            std::vector<Display::Point> *points = new std::vector<Display::Point>;
            // points_->clear();

            int num_points = json["num_points"];
            Serial.printf("Received %d points\n", num_points);
            for (int i = 0; i < num_points; i++)
            {
                Serial.printf("Point %d: %f, %f\n", i, json[String(i)]["x"], json[String(i)]["y"]);
                points->push_back(Display::Point{json[String(i)]["x"], json[String(i)]["y"]});
            }
            display_->SetImage(points);
            Serial.println("Updated image");
            aws_data.clear();
        }
    }

    void notifyClients()
    {
        // NOthing here unless we want to use it
        // ws.textAll(led.on ? "on" : "off");
    }
};
