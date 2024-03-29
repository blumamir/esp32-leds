#define CONFIG_USE_ONLY_LWIP_SELECT 1

#include <Arduino.h>
#include <WiFi.h>
#include <secrets.h>
#include <PubSubClient.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#include <animations/i_animation.h>
#include <render_utils.h>
#include <song_offset_tracker.h>
#include <animations_container.h>
#include <fs_manager.h>
#include <animation_factory.h>
#include <segments_store.h>
#include <protobuf_infra.h>

#include "SPIFFS.h"
#include <WebServer.h>
#include <InfluxDbClient.h>
// InfluxDB  server url. Don't use localhost, always server name or ip address.
// For InfluxDB 1 e.g. http://192.168.1.48:8086
#define INFLUXDB_URL "http://10.0.0.200:8086"
// InfluxDB v1 database name 
#define INFLUXDB_DB_NAME "kivsee"
// InfluxDB v1 username
//#define INFLUXDB_USER "kivsee"
// InfluxDB v1 password
//#define INFLUXDB_PASSWORD "kivsee12"
// InfluxDB client instance for InfluxDB 1
InfluxDBClient influxClient(INFLUXDB_URL, INFLUXDB_DB_NAME);
// Data point
Point sensor("wifi_status");

#ifndef NUM_LEDS
#warning NUM_LEDS not definded. using default value of 300
#define NUM_LEDS 300
#endif // NUM_LEDS

#ifndef MQTT_BROKER_PORT
#define MQTT_BROKER_PORT 1883
#endif //MQTT_BROKER_PORT

#ifndef MONITOR_TOPIC_PREFIX
#define MONITOR_TOPIC_PREFIX "monitor"
#endif // MONITOR_TOPIC_PREFIX
String monitorTopic(MONITOR_TOPIC_PREFIX "/");

#define MAX_THING_NAME_LENGTH 16
char thing_name[MAX_THING_NAME_LENGTH];

const unsigned int WD_TIMEOUT_MS = 2000;

HSV leds_hsv[NUM_LEDS];
RenderUtils renderUtils(leds_hsv, NUM_LEDS);
SongOffsetTracker songOffsetTracker;
AnimationsContainer animationsContainer;
FsManager fsManager;
SegmentsStore segmentsStore;

TaskHandle_t Task1;

WebServer server(80);

struct NewSongMsg {
  bool onlyUpdateTime;
  const AnimationsContainer::AnimationsList *anList;
  int32_t songStartTime;
};
QueueHandle_t anListQueue;
const int anListQueueSize = 10;

QueueHandle_t deleteAnListQueue;
const int deleteAnListQueueSize = 10;

QueueHandle_t wdQueue;
const int wdQueueSize = 10;

void PrintCorePrefix()
{
  Serial.print("["); Serial.print(xPortGetCoreID()); Serial.print("]: ");
}

StaticJsonDocument<40000> doc;

void Core0WDSend(unsigned int currMillis) 
{
  static unsigned int lastWdSendTime = 0;
  if(currMillis - lastWdSendTime > WD_TIMEOUT_MS) {
    Serial.println("[0] send wd msg from core 0 to core 1");
    int unused = 0;
    xQueueSend(wdQueue, &unused, 5);
    lastWdSendTime = currMillis;
  }
}

void Core0WdReceive(unsigned int currMillis) 
{
  static unsigned int lastCore0WdReceiveTime = 0;
  int unused;
  if(xQueueReceive(wdQueue, &unused, 0) == pdTRUE) {
    lastCore0WdReceiveTime = currMillis;
  }

  unsigned int timeSinceWdReceive = currMillis - lastCore0WdReceiveTime;
  // Serial.print("[0] timeSinceWdReceive: ");
  // Serial.println(timeSinceWdReceive);
  if(timeSinceWdReceive > (3 * WD_TIMEOUT_MS)) {
    ESP.restart();
  }
}

void SendAnListUpdate()
{
    NewSongMsg msg;
    if(songOffsetTracker.IsSongPlaying()) {
      String currFileName = songOffsetTracker.GetCurrentFile();
      PrintCorePrefix(); Serial.print("currFileName: ");
      Serial.println(currFileName);
      msg.songStartTime = songOffsetTracker.GetSongStartTime();
      msg.onlyUpdateTime = false;
      if(msg.songStartTime != 0) {
        msg.anList = animationsContainer.SetFromJsonFile(currFileName, doc, segmentsStore);
      }
      else {
        PrintCorePrefix(); Serial.println("ignoring an list update since song start time is not valid yet");
        msg.anList = nullptr;
      }
    }
    else {
      PrintCorePrefix(); Serial.println("no song is playing");
      msg.anList = nullptr;
      msg.songStartTime = 0;
      msg.onlyUpdateTime = false;
    }

    xQueueSend(anListQueue, &msg, portMAX_DELAY);
}

void SendStartTimeToRenderCore()
{
  if(!songOffsetTracker.IsSongPlaying())
    return;

  NewSongMsg msg;
  msg.onlyUpdateTime = true;
  msg.songStartTime = songOffsetTracker.GetSongStartTime();
  msg.anList = nullptr;
  PrintCorePrefix(); Serial.println("updateing time of current song start");
  xQueueSend(anListQueue, &msg, portMAX_DELAY);
}

void callback(char* topic, byte* payload, unsigned int length) {

  Serial.print("Message arrived, ");
  Serial.print(length);
  Serial.print(" [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  if (strncmp("animations/", topic, 11) == 0) {
    int songNameStartIndex = 11 + strlen(thing_name) + 1;
    String songName = String(topic + songNameStartIndex);
    fsManager.SaveToFs((String("/music/") + songName).c_str(), payload, length);

    if(songOffsetTracker.GetCurrentFile() == songName) {
      SendAnListUpdate();
    }
    
  } else if(strcmp("current-song", topic) == 0) {
    songOffsetTracker.HandleCurrentSongMessage((char *)payload);
    SendAnListUpdate();

  } else if(strncmp("objects-config", topic, 14) == 0) {
    fsManager.SaveToFs("/objects-config", payload, length);
    ESP.restart();
  }

  Serial.print("[0] done handling mqtt callback: ");
  Serial.println(topic);
}

void ConnectToWifi() {

  if (WiFi.status() == WL_CONNECTED)
    return;

  while(true) {
    unsigned int connectStartTime = millis();
    WiFi.disconnect();
    WiFi.mode(WIFI_STA);
    WiFi.begin(SSID, WIFI_PASSWORD);
    Serial.printf("Attempting to connect to SSID: ");
    Serial.printf(SSID);
    while (millis() - connectStartTime < 10000)
    {
        Serial.print(".");
        // Core0WDSend(millis());
        delay(1000);
        // Core0WDSend(millis());
        if(WiFi.status() == WL_CONNECTED) {
          Serial.println("connected to wifi");
          return;
        }
    }
    Serial.println(" could not connect for 10 seconds. retry");
  }
}

WiFiClient net;
PubSubClient client(net);
void ConnectToMessageBroker() {

    if(client.connected())
      return;

    client.setServer(MQTT_BROKER_IP, MQTT_BROKER_PORT);
    client.setCallback(callback);
    StaticJsonDocument<128> json_doc;
    json_doc["ThingName"] = thing_name;
    json_doc["Alive"] = false;
    char lastWillMsg[128];
    serializeJson(json_doc, lastWillMsg);
    Serial.println("connecting to mqtt");
    if(client.connect(thing_name, monitorTopic.c_str(), 1, true, lastWillMsg)) {
        Serial.println("connected to message broker");
        client.subscribe((String("objects-config/") + String(thing_name)).c_str(), 1);
        client.subscribe("current-song", 1);
        client.subscribe((String("animations/") + String(thing_name) + String("/#")).c_str(), 1);
    }
    else {
        Serial.print("mqtt connect failed. error state:");
        Serial.println(client.state());
    }

}

void HandleObjectsConfig(File &f) 
{
  pb_istream_t pbInputStream = FileToPbStream(f);
  bool totalSegments = segmentsStore.InitFromFile(leds_hsv, pbInputStream);
  if(totalSegments)
  {
    Serial.print("successfully parsed and initialized segments store. ");
    Serial.print("found "); Serial.print(segmentsStore.GetNumberOfSegments()); Serial.println(" segments");
  }
  else
  {
    Serial.print("failed to parse and initialize segments store. error: ");
    const char *errorMsg = segmentsStore.GetErrorDescription();
    Serial.println(errorMsg ? errorMsg : "cannot be determined");
  }
}

void DeleteAnListPtr() {
  const AnimationsContainer::AnimationsList *ptrToDelete;
  if(xQueueReceive(deleteAnListQueue, &ptrToDelete, 0) == pdTRUE) {
    for(IAnimation *an: (*ptrToDelete)) {
      delete an;
    }
    delete ptrToDelete;
  }
}

void SendMonitorMsg(char *buffer, size_t bufferSize) {

  StaticJsonDocument<128> json_doc;
  json_doc["ThingName"] = thing_name;
  json_doc["Alive"] = true;
  json_doc["WifiSignal"] = WiFi.RSSI();
  serializeJson(json_doc, buffer, bufferSize);
}

void handle_Prometheus() {
  Serial.println("got client http");
  char buf[1024];
  sprintf(buf, "wifi_signal_strength %d\nuptime %d\nfree_heap %d\n", WiFi.RSSI(), millis(), esp_get_free_heap_size());
  // String metric1 = String("wifi_signal_strength: ") + WiFi.RSSI() + "\n";
  server.send(200, "text/plain", buf);
}

void MonitorLoop( void * parameter) {

  ConnectToWifi();

  server.begin();
  server.on("/metrics", handle_Prometheus);

  // Port defaults to 3232
  // ArduinoOTA.setPort(3232);

  // Hostname defaults to esp3232-[MAC]
  ArduinoOTA.setHostname(thing_name);

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type);
    })
    .onEnd([]() {
      Serial.println("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });

  ArduinoOTA.begin();
  songOffsetTracker.setup();
  MDNS.addService("prometheus-http", "tcp", 80); // Announce esp tcp service on port 80
  MDNS.addService("http", "tcp", 80);

  // Set InfluxDB 1 authentication params
  //influxClient.setConnectionParamsV1(INFLUXDB_URL, INFLUXDB_DB_NAME, INFLUXDB_USER, INFLUXDB_PASSWORD);
  // Add constant tags - only once
  sensor.addTag("device", thing_name);
  //sensor.addTag("SSID", WiFi.SSID());

  // Check server connection
  if (influxClient.validateConnection()) {
    Serial.print("Connected to InfluxDB: ");
    Serial.println(influxClient.getServerUrl());
  } else {
    Serial.print("InfluxDB connection failed: ");
    Serial.println(influxClient.getLastErrorMessage());
  }

  unsigned int lastReportTime = millis();
  unsigned int lastMonitorTime = millis();
  unsigned int lastUpdateDBTime = millis();
  for(;;) {
    DeleteAnListPtr();
    ConnectToWifi();
    ConnectToMessageBroker();
    unsigned int currTime = millis();
    // Core0WDSend(currTime);
    if (currTime - lastMonitorTime >= 1000) {
      char monitorMsg[128];
      SendMonitorMsg(monitorMsg, 128);
      client.publish(monitorTopic.c_str(), monitorMsg, true);
      lastMonitorTime = currTime;
    }
    if(currTime - lastReportTime >= 5000) {
      Serial.print("[0] current millis: ");
      Serial.println(millis());
      Serial.print("[0] wifi client connected: ");
      Serial.println(WiFi.status() == WL_CONNECTED);
      Serial.print("[0] mqtt client connected: ");
      Serial.println(client.connected());
      lastReportTime = currTime;
    }
    client.loop();

    // influxDB write
    if (currTime - lastUpdateDBTime >= 3000) {
      // Store measured value into point
      sensor.clearFields();
      // Report RSSI of currently connected network
      sensor.addField("rssi", WiFi.RSSI());
      sensor.addField("uptime", millis());
      sensor.addField("free heap", esp_get_free_heap_size());
      // Print what are we exactly writing
      Serial.print("Writing: ");
      Serial.println(sensor.toLineProtocol());
      // Write point
      if (!influxClient.writePoint(sensor)) {
        Serial.print("InfluxDB write failed: ");
        Serial.println(influxClient.getLastErrorMessage());
      }
      lastUpdateDBTime = currTime;
    }

    bool clockChanged, clockFirstValid;
    songOffsetTracker.loop(&clockChanged, &clockFirstValid);
    if(clockChanged)
    {
      if(clockFirstValid)
      {
        SendAnListUpdate();
      }
      else
      {
        SendStartTimeToRenderCore();
      }
    }
    ArduinoOTA.handle();

    server.handleClient();

    vTaskDelay(5);
  }
}

NewSongMsg msg;

void setup() {
  Serial.begin(115200);
  disableCore0WDT();

  msg.anList = nullptr;
  msg.songStartTime = 0;

  anListQueue = xQueueCreate( anListQueueSize, sizeof(NewSongMsg) );
  deleteAnListQueue = xQueueCreate( deleteAnListQueueSize, sizeof(const AnimationsContainer::AnimationsList *) );
  wdQueue = xQueueCreate( wdQueueSize, sizeof(int) );

  fsManager.setup();
  renderUtils.Setup();

  bool hasThingName = fsManager.ReadThingName(thing_name, MAX_THING_NAME_LENGTH);
  if(!hasThingName) 
  {
    Serial.println("Thing name not configured - upload file to continue");
    return;
  }
  Serial.print("Thing name: "); Serial.println(thing_name);
  monitorTopic += thing_name;
  Serial.print("Mqtt monitor topic is: "); Serial.println(thing_name);

  File file = SPIFFS.open("/objects-config");
  if(file){
      HandleObjectsConfig(file);
      file.close();
  }
  else {
      Serial.println("Failed to open objects config file for reading");
  }

  xTaskCreatePinnedToCore(
      MonitorLoop, /* Function to implement the task */
      "MonitorTask", /* Name of the task */
      16384,  /* Stack size in words */
      NULL,  /* Task input parameter */
      0,  /* Priority of the task */
      &Task1,  /* Task handle. */
      0); /* Core where the task should run */

}

// we keep this object once so we don't need to create the string on every loop
CurrentSongDetails songDetails;

unsigned int lastPrint1Time = millis();

void loop() {

  unsigned long currentMillis = millis();
  // Core0WdReceive(currentMillis);

  if(currentMillis - lastPrint1Time >= 5000) {
    Serial.println("[1] core 1 alive");
    lastPrint1Time = currentMillis;
  }

  NewSongMsg newMsg;
  if(xQueueReceive(anListQueue, &newMsg, 0) == pdTRUE) {
    Serial.println("[1] received message on queue");
    Serial.print("[1] songStartTime: ");
    Serial.println(newMsg.songStartTime);
    Serial.print("[1] an list valid: ");
    Serial.println(newMsg.anList != nullptr);
    Serial.print("[1] only update time: ");
    Serial.println(newMsg.onlyUpdateTime);

    if(newMsg.onlyUpdateTime) {
      msg.songStartTime = newMsg.songStartTime;      
    }
    else {
      if(msg.anList != nullptr) {
        Serial.println("sending animation ptr for deleteing to core 0");
        xQueueSend(deleteAnListQueue, &msg.anList, portMAX_DELAY);
      }
      msg = newMsg;
    }
  }

  renderUtils.Clear();

  bool hasValidSong = msg.anList != nullptr;
  if(hasValidSong) {
    int32_t songOffset = ((int32_t)(currentMillis)) - msg.songStartTime;
    const AnimationsContainer::AnimationsList *currList = msg.anList;
    // Serial.print("number of animations: ");
    // Serial.println(currList->size());
    // Serial.print("song offset: ");
    // Serial.println(songOffset);
    for(AnimationsContainer::AnimationsList::const_iterator it = currList->begin(); it != currList->end(); it++) {
      IAnimation *animation = *it;
      if(animation != nullptr && animation->IsActive(songOffset)) {
        animation->Render((unsigned long)songOffset);
      }
    }
  }
  // unsigned long renderLoopTime = millis() - currentMillis;
  // Serial.print("loop time ms: ");
  // Serial.println(renderLoopTime);

  renderUtils.Show();

  vTaskDelay(5);
}
