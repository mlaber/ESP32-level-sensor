//Zisterne Fuellstand

//*********************************************************************************************************
//
// first release on 05/2021
// Version 0020 , July.13 2021
//


// THE SOFTWARE IS PROVIDED "AS IS" FOR PRIVATE USE ONLY, IT IS NOT FOR COMMERCIAL USE IN WHOLE OR PART OR CONCEPT.
// FOR PERSONAL USE IT IS SUPPLIED WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
// WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHOR
// OR COPYRIGHT HOLDER BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
// OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE
//
//

//**************************************************************************************************
//                                           LIBRARIES                                             *
//**************************************************************************************************


#include <Arduino.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <WebServer.h>
#include <prometheus_metric.h>
#include <esp_task_wdt.h>



//**************************************************************************************************
//                           PIN MAPPING                                                           *
//**************************************************************************************************
//GPIO22    Ultrasonoc Trigger pin
//GPIO23    Ultrasonic Echo pin
//GPIO32    Supply voltage / battey voltage sensor (optional)  ADC2_CH4  connect via 10k resistors in a voltage provider 5V VCC = 


int triggerPin = 22;
int echoPin = 23;
int VCCsensor = 32;

//**************************************************************************************************
//                                    VARIABLES AND CONSTANTS                                       *
//**************************************************************************************************



// Enter your parameters here: //
const char* ssid = "Your-WiFi-SSID";                // WLAN SSID
const char* password = "Your-WiFi-password";                    // WLAN password
const char* hostname ="zisterne01";            // DNS hostname of the ESP also used as prefix for prometheus metric name
int port = 9110;                              // TCP port for prometheus exporter webserver see https://github.com/prometheus/prometheus/wiki/Default-port-allocations for default ports.
int ota_port = 3232;                          // TCP port for OTA Updates
int samplerate = 10;                          // 1..10 Samples that ave to been taken to calculate a average metric
int watchdog_timeout = 90;                    // Seconds before a blocking task leads to a watchdog reset
float min_level = 173.0;                       // distance to level of fluid in the reservoir in cm, the level where the pump dry-run protection shuts down the pump
float max_level = 28.0;                      // distance level of fluid in the reservoir in cm, the level where the fluid is drained by the overflow 
// End of parameters //

float version = 0.20;
float level_percentage = 0.0;
float cmDistance;     // Distance from the sensor to the surface of the fluid
float cmHeight;       // Height of the fluid in reservoir
float distance; 
float vcc = 0.0;            // calcutlated supply voltage from ESP ADC.
unsigned long duration;
unsigned long avg_duration;
long rssi;           // WiFi RSSI signal strength
float uptime;        // Controller Uptime



Metric distance2object(MetricType::gauge, "distance_to_object", "Distance from the sensor to the surface of the fluid in cm", 1, hostname); 
Metric fwversion(MetricType::gauge, "fw_version", "Firmware Version", 2, hostname); // 2 means to decimal places, hostname is the prefix for prometheus name
Metric fillstatepercentage(MetricType::gauge, "fillstate_percentage", "Fillstate percentage", 2, hostname); // 2 means to decimal places
Metric wifirssi(MetricType::gauge, "wifi_rssi", "WiFi receive signal strenght dBm", 1, hostname); 
Metric supplyvcc(MetricType::gauge, "supply_vcc", "Supply VCC voltage milliVolt", 2, hostname ); 
Metric controller_uptime(MetricType::gauge, "uptime", "Uptime of the controller Seconds", 2, hostname); 

WebServer server(port);


//**************************************************************************************************
//                                          FUNCTIONS                                              *
//**************************************************************************************************

long getUptime() {
  long uptime = esp_timer_get_time() / 1000000;   // https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/esp_timer.html
    //Serial.println("------------------------------------");
    //Serial.print  ("Controller uptime       "); Serial.print(uptime); Serial.println(" seconds");
    //Serial.println("------------------------------------"); 
    return(uptime);
}


long getWiFiRSSI() {
  rssi = WiFi.RSSI();
  Serial.println("------------------------------------");
  Serial.print  ("WiFi Signal strength:       "); Serial.print(rssi); Serial.println(" dBm");
  Serial.println("------------------------------------"); 
  return(rssi);
}


int getVCCvoltage(){
  float vcc_raw = analogReadMilliVolts(VCCsensor);
  vcc = (vcc_raw * 2.0); // * 2.0  because the voltage divider, divides the currency on the GPIO pin by 2
  Serial.println("------------------------------------");
  Serial.println("Supply voltage:       "); Serial.print(vcc); Serial.println(" mV");
  Serial.println("------------------------------------"); 
  return(vcc);
}

int getUltrasonicDistance(){
  avg_duration = 0;
  int i = 0;
  int non_zero_value = 0;
  while(i < samplerate) {
    digitalWrite(triggerPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(triggerPin, LOW);
    duration = pulseIn(echoPin, HIGH);

    if (duration > 0)   {     // 0 Values are physical very unlikely and should be ignored
      non_zero_value ++;
      avg_duration = avg_duration + duration;
    }  
    delay(30);              // 30 ms =  102 meters distance
    i++;
    }

  // Speed of sound in air: 343m/S (at 20°C)
  // 343,2 m/S --> 34,32cm / ms --> 0,03432cm / µs
  // divided by 2 because of the relection from the surface
  cmDistance = (avg_duration / non_zero_value) * 0.03432 / 2; 
  Serial.println("------------------------------------");
  Serial.print  ("Non Zero samples: "); Serial.print(non_zero_value); Serial.print  (" of total samples: "); Serial.println(samplerate);
  Serial.println("------------------------------------");

  Serial.println("------------------------------------");
  Serial.print  ("Distance:       "); Serial.println(cmDistance);
  Serial.println("------------------------------------");
  level_percentage = 100.0 / (min_level - max_level) * (min_level - cmDistance);
  Serial.println("------------------------------------");
  Serial.print  ("Fillstate:       "); Serial.print(level_percentage); Serial.println("%");
  Serial.println("------------------------------------"); 
  delay(200);
  return cmDistance;
  }

  

void handleRoot() {
  server.send(200, "text/html", "<a href=\"/metrics\">metrics</a>");
}

void handleMetrics() {                // Provide prometheus metrics under /metrics - the expected URI for the prometheus scraper
  String response = "";
  distance = getUltrasonicDistance();
  distance2object.setValue(distance);
  fwversion.setValue(version);
  fillstatepercentage.setValue(level_percentage);
  long rssi = getWiFiRSSI();
  wifirssi.setValue(rssi);
  vcc = getVCCvoltage();
  supplyvcc.setValue(vcc);
  uptime = getUptime();
  controller_uptime.setValue(uptime);

  response = distance2object.getString() + fwversion.getString() + fillstatepercentage.getString() + wifirssi.getString() + supplyvcc.getString() + controller_uptime.getString(); // Concat al the metrics
  server.send(200, "text/plain; version=0.0.4", response); 
}

//**************************************************************************************************
//                                           S E T U P                                             *
//**************************************************************************************************

void setup() {
  Serial.begin(115200);

  Serial.println("Configuring ESP32 task watchdog (WDT)...");     //https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/wdts.html
  esp_task_wdt_init(watchdog_timeout, true); //enable panic so ESP32 restarts
  esp_task_wdt_add(NULL); //add current thread to WDT watch
  pinMode(triggerPin, OUTPUT);
  pinMode(echoPin, INPUT);


  Serial.print("Version: ");
  Serial.println(version);

  Serial.println("Initializing sensor");
  getUltrasonicDistance();
  getWiFiRSSI();
  getVCCvoltage();


  // Connect WiFi
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE); // workarround to use hostname for dhcp client
  WiFi.mode(WIFI_STA);
  WiFi.hostname(hostname);
  WiFi.begin(ssid, password);

  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

 
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
  ArduinoOTA.setPort(ota_port);
  ArduinoOTA.setHostname(hostname);
  ArduinoOTA.begin();
  Serial.print("OTA Port: ");
  Serial.println(ota_port); 
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("Prometheus exporter URL: http://");
  Serial.print(hostname);
  Serial.print(".local:");
  Serial.println(port);
  Serial.println("Ready");


  server.on("/", handleRoot);
  server.on("/metrics", handleMetrics);
  server.begin();
  Serial.printf("HTTP server started on port %d\n",port);
}



//**************************************************************************************************
//                                          MAIN                                                   *
//**************************************************************************************************
  
void loop(){
  ArduinoOTA.handle();     // Handler for Over The Air firmware updates
  esp_task_wdt_reset();     // Reset the task watchdog because we are still runnning ...
  server.handleClient();
}

void handleRoot();

void handleMetrics();
