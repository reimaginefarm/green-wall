
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPClient.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#include <ArduinoJson.h>

#include <TaskScheduler.h>

#define PWMRANGE 1023

bool enableDebugging = true;

////////////////////////////////////////////////////////
// slave unit vars begin:
const String deviceType = "fogger";
const String deviceId = "1";

const int transistorPin = 14;
int transistorPinPwmValue = 1023;

unsigned long dataUpdateInterval = 2000; // in milliseconds

// Enable either deepsleep or wifi off or none
bool enableDeepSleep = false;
bool enableWifiOffWhenNotUsed = false;

// slave unit vars end;
////////////////////////////////////////////////////////


////////////////////////////////////////////////////////
// Update these with values suitable for your network.
// cnk vars begin:
unsigned long currentTime;
const char* ssid = "2.4g";
const char* password = "dauyndauyn";
// cnk vars end;
////////////////////////////////////////////////////////


////////////////////////////////////////////////////////
// http request vars begin:
String masterHostName = "vaponicwalltestpi";
String phpFile = "index.php";
String query;
String payload;

// Default values
int deviceIsEnabled = 0;
int runOnlyOnce = 1;
int powerPercent = 0;
unsigned long serverDataUpdateInterval = 2;
int serverEnableDeepSleep = 0;
int serverEnableWifiOffWhenNotUsed = 0;
long serverCurrentUnixTime;
unsigned long onDuration = 5;
unsigned long offDuration = 5;
// http request vars end;
////////////////////////////////////////////////////////


////////////////////////////////////////////////////////
// blink vars begin:
const int ESP_BUILTIN_LED = 2; // 2- blue, 0 - red
unsigned long thisLoop = millis();
unsigned long blinkInterval = 1000;
bool ledState = HIGH;
bool blinking = false;
// blink vars end;
////////////////////////////////////////////////////////

void setup_wifi() {

        // wifi network cnk begin:
        Serial.println();
        Serial.print("Connecting to ");
        Serial.println(ssid);

        if(enableWifiOffWhenNotUsed) {
                // Disable the WiFi persistence.  The ESP8266 will not load and save WiFi settings in the flash memory.
                WiFi.persistent( false );
        }

        WiFi.hostname("vaponicwall-" + deviceType + "-" + deviceId);

        WiFi.begin(ssid, password);
        while (WiFi.status() != WL_CONNECTED) {
                delay(200);
                Serial.print(".");
        }

        randomSeed(micros());

        Serial.println("");
        Serial.println("WiFi connected");
        Serial.println("IP address: ");
        Serial.println(WiFi.localIP());
        // wifi network cnk end;
}

void setup() {

        ////////////////////////////////////////////////////////
        //////////////// SLAVE UNIT SETUP BEGIN ////////////////
        ////////////////////////////////////////////////////////
        pinMode(transistorPin, OUTPUT);
        digitalWrite(transistorPin, LOW);
        ////////////////////////////////////////////////////////
        ////////////// SLAVE UNIT SETUP COMPLETE ///////////////
        ////////////////////////////////////////////////////////


        ////////////////////////////////////////////////////////
        //////////////// WIFI - OTA SETUP BEGIN ////////////////
        ////////////////////////////////////////////////////////
        Serial.begin(115200);
        Serial.println("Booting");

        delay(10);

        setup_wifi(); // wifi cnk procedure;

        pinMode(ESP_BUILTIN_LED, OUTPUT);

        // ota setup begin:

        // Port defaults to 8266
        // ArduinoOTA.setPort(8266);

        // Hostname defaults to esp8266-[ChipID]
        String slaveHostName =  "vaponicwall " + deviceType + " " + deviceId;

        // Length (with one extra character for the null terminator)
        int str_len = slaveHostName.length() + 1;

        // Prepare the character array (the buffer)
        char char_array[str_len];

        slaveHostName.toCharArray(char_array, str_len);

        ArduinoOTA.setHostname(char_array);

        // No authentication by default
        // ArduinoOTA.setPassword((const char *)"123");


        ArduinoOTA.onStart([]() {
                Serial.println("Start");
        });
        ArduinoOTA.onEnd([]() {
                Serial.println("\nEnd");
        });
        ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
                Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
        });
        ArduinoOTA.onError([](ota_error_t error) {
                Serial.printf("Error[%u]: ", error);
                if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
                else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
                else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
                else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
                else if (error == OTA_END_ERROR) Serial.println("End Failed");
        });
        ArduinoOTA.begin();

        // ota setup end;

        ///////////////////////////////////////////////////////
        ////////////// WIFI - OTA SETUP COMPLETE //////////////
        ///////////////////////////////////////////////////////

        ///////////////////////////////////////////////////////
        ////////////////// SLAVE SETUP BEGIN //////////////////
        ///////////////////////////////////////////////////////

        // A line break for more readibility
        Serial.println(" ");
        Serial.println("Connecting to server to get slave configuration");
        Serial.println("Please make sure that the following variables are correct for this slave device");
        Serial.print("Device Type: ");
        Serial.println(deviceType);
        Serial.print("Device ID: ");
        Serial.println(deviceId);

        updateVariablesFromDb();

        if(enableWifiOffWhenNotUsed) {
                wifiTurnOff();
        }

        ///////////////////////////////////////////////////////
        //////////////// SLAVE SETUP COMPLETE /////////////////
        ///////////////////////////////////////////////////////

}

bool httpRequestParser() {
        // Declare an object of class HTTPClient
        HTTPClient http;

        // Create a query211qw
        query = "http://" + masterHostName + ".local/" + phpFile + "?deviceType=" + deviceType + "&deviceId=" + deviceId + "&deviceIp=" + WiFi.localIP().toString();

        if(enableDebugging) {
                // A line break for more readibility
                Serial.println(" ");
                Serial.println("HTTP query is:");
                Serial.println(query);
        }

        //Specify request destination
        http.begin(query);

        // Send the request
        int httpCode = http.GET();

        // Check the returning code
        // httpCode > 0
        if (true) {

                // Parsing
                const size_t bufferSize = JSON_OBJECT_SIZE(9) + 200;
                DynamicJsonBuffer jsonBuffer(bufferSize);

                const char* json = "{\"deviceIsEnabled\":1,\"runOnlyOnce\":0,\"powerPercent\":100,\"dataUpdateInterval\":3,\"enableDeepSleep\":0,\"enableWifiOffWhenNotUsed\":0,\"serverCurrentUnixTime\":1533904819,\"onDuration\":5,\"offDuration\":10}";

                JsonObject& root = jsonBuffer.parseObject(http.getString());

                deviceIsEnabled = root["deviceIsEnabled"]; // 0
                runOnlyOnce = root["runOnlyOnce"]; // 0
                powerPercent = root["powerPercent"]; // 100
                serverDataUpdateInterval = root["dataUpdateInterval"]; // 2
                serverEnableDeepSleep = root["enableDeepSleep"]; // 0
                serverEnableWifiOffWhenNotUsed = root["enableWifiOffWhenNotUsed"]; // 0
                serverCurrentUnixTime = root["serverCurrentUnixTime"];                 // 1533838233
                onDuration = root["onDuration"]; // -1
                offDuration = root["offDuration"]; // -1

                // For debugging
                if(enableDebugging) {

                        // A line break for more readibility
                        Serial.println(" ");

                        Serial.print("Parsed variables from HTTP response:");
                        Serial.println(" ");

                        // Output to serial monitor
                        Serial.print("deviceIsEnabled:");
                        Serial.println(deviceIsEnabled);

                        Serial.print("runOnlyOnce:");
                        Serial.println(runOnlyOnce);

                        Serial.print("powerPercent:");
                        Serial.println(powerPercent);

                        Serial.print("onDuration (seconds):");
                        Serial.println(onDuration);

                        Serial.print("offDuration (seconds):");
                        Serial.println(offDuration);

                        Serial.print("serverDataUpdateInterval (seconds):");
                        Serial.println(serverDataUpdateInterval);

                        Serial.print("serverEnableDeepSleep:");
                        Serial.println(serverEnableDeepSleep);

                        Serial.print("serverEnableWifiOffWhenNotUsed:");
                        Serial.println(serverEnableWifiOffWhenNotUsed);

                        Serial.print("serverCurrentUnixTime:");
                        Serial.println(serverCurrentUnixTime);

                        // A line break for more readibility
                        Serial.println(" ");
                }

                // For debugging to see server response
                if (true) {
                        // Get the request response payload
                        payload = http.getString();
                        // Print the query
                        Serial.println("Query: " + query);

                        // Print the response payload
                        Serial.println("Response: " + payload);

                        // A line break for more readibility
                        Serial.println(" ");
                }

                // Stop blinking to indicate that there is no problem
                blinking = false;

                // Close connection
                http.end();

                return true;
        } else {

                // For debugging
                if(enableDebugging) {
                        // A line break for more readibility
                        Serial.println(" ");
                        Serial.println("////////////////////////////////////////////////");

                        Serial.println("SERVER CONNECTION ERROR");

                        Serial.print("HTTP ERROR CODE IS:");
                        Serial.println(httpCode);

                        // A line break for more readibility
                        Serial.println(" ");
                        payload = http.getString();
                        // Print the response payload
                        Serial.println("HTTP Response: " + payload);

                        Serial.println("////////////////////////////////////////////////");
                        // A line break for more readibility
                        Serial.println(" ");
                }

                // Start blinking to indicate the problem physically
                blinking = true;

                // Close connection
                http.end();

                ////////////////////////////////////////////////////////////////
                ////////////////////////////////////////////////////////////////
                ESP.restart();
                ////////////////////////////////////////////////////////////////
                ////////////////////////////////////////////////////////////////

                return false;
        }



}

void updateVariablesFromDb(){
        // html request begin:
        if (WiFi.status() == WL_CONNECTED) {
                if (currentTime - thisLoop >= dataUpdateInterval) {
                        thisLoop = currentTime;

                        // Send the http request to server and get the variables
                        bool isSuccessfull = httpRequestParser();

                        ///////////////////////////////////////////////////////
                        //////////// VARIABLE CONVERSATION - BEGIN ////////////
                        ///////////////////////////////////////////////////////

                        // Value mapping to PWM value for the power percent value
                        transistorPinPwmValue = map(powerPercent, 0, 100, 0, 1023);

                        // convert seconds to milliseconds for millis()
                        dataUpdateInterval = serverDataUpdateInterval*1000;

                        // convert seconds to milliseconds for millis()
                        onDuration = onDuration*1000;

                        // convert seconds to milliseconds for millis()
                        offDuration = offDuration*1000;

                        // Convert to boolean variable
                        if(serverEnableDeepSleep == 1) {
                                enableDeepSleep = true;
                        } else if(serverEnableDeepSleep == 0) {
                                enableDeepSleep = false;
                        }

                        // Convert to boolean variable
                        if(serverEnableWifiOffWhenNotUsed == 1) {
                                enableWifiOffWhenNotUsed = true;
                        } else if(serverEnableWifiOffWhenNotUsed == 0) {
                                enableWifiOffWhenNotUsed = false;
                        }

                        ///////////////////////////////////////////////////////
                        ///////////// VARIABLE CONVERSATION - END /////////////
                        ///////////////////////////////////////////////////////

                        if((enableDebugging) and (isSuccessfull)) {

                                Serial.println("Updated variables successfully");
                        }
                }
        } else {
                if(enableDebugging) {
                        // A line break for more readibility
                        Serial.println(" ");
                        Serial.println("Not connected to Wifi, trying to connect again!");
                }
                setup_wifi();
        }
        // html request end;

}

void wifiTurnOff(){
        if(enableDebugging) {
                // A line break for more readibility
                Serial.println(" ");
                Serial.println("TURNED OFF WIFI");
                Serial.println("////////////////////////////////////////////////");
        }

        // Turn WIFI off
        WiFi.mode( WIFI_OFF );
        WiFi.forceSleepBegin();
        yield();
}

void wifiTurnOn(){
        if(enableDebugging) {
                // A line break for more readibility
                Serial.println(" ");
                Serial.println("////////////////////////////////////////////////");
                Serial.println("TURNED ON WIFI");
        }

        WiFi.forceSleepWake();
        yield();

        // Disable the WiFi persistence.  The ESP8266 will not load and save WiFi settings in the flash memory.
        WiFi.persistent( false );

        // Bring up the WiFi connection
        WiFi.mode( WIFI_STA );

        setup_wifi();

        //yield();
}

void loop() {

        ArduinoOTA.handle();

        currentTime = millis();

        ////////////////////////////////////////////////////////
        ////////////////// SLAVE LOOP - BEGIN //////////////////
        ////////////////////////////////////////////////////////
        if ((deviceIsEnabled == 1) and (powerPercent != 0)) {

                analogWrite(transistorPin, transistorPinPwmValue);

        } else {
                analogWrite(transistorPin, 0);

                if(enableDeepSleep) {
                        // Converting milliseconds to microseconds
                        unsigned long sleepDuration = dataUpdateInterval*1000;

                        if(enableDebugging) {
                                // A line break for more readibility
                                Serial.println(" ");
                                Serial.print("Going into deep sleep for ");
                                Serial.print(sleepDuration);
                                Serial.println(" microseconds");
                        }

                        // Sleep the esp8266 chip
                        ESP.deepSleep(sleepDuration);
                }

        }
        ////////////////////////////////////////////////////////
        /////////////////// SLAVE LOOP - END ///////////////////
        ////////////////////////////////////////////////////////


        ////////////////////////////////////////////////////////
        //////////// SLAVE DATA UPDATE LOOP - BEGIN ////////////
        ////////////////////////////////////////////////////////
        if (currentTime - thisLoop >= dataUpdateInterval) {
                //thisLoop = currentTime;
                if(enableWifiOffWhenNotUsed) {
                        wifiTurnOn();
                }

                updateVariablesFromDb();

                if(enableWifiOffWhenNotUsed) {
                        wifiTurnOff();
                }

        }
        ////////////////////////////////////////////////////////
        ///////////// SLAVE DATA UPDATE LOOP - END /////////////
        ////////////////////////////////////////////////////////


        ////////////////////////////////////////////////////////
        //////////////////// BLINK - BEGIN /////////////////////
        ////////////////////////////////////////////////////////
        // LED blinks when there is a server connection error
        if (blinking == true and (currentTime - thisLoop >= blinkInterval)) {
                thisLoop = currentTime;
                if (ledState == LOW) {
                        ledState = HIGH;
                } else ledState = LOW;
                digitalWrite(ESP_BUILTIN_LED, ledState);
        }
        ////////////////////////////////////////////////////////
        ///////////////////// BLINK - END //////////////////////
        ////////////////////////////////////////////////////////

}
