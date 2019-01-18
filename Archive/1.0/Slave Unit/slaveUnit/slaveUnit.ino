/*
 ##############################################
 # Team NYUAD, Green / Vaponic Wall Code v1.0 #
 # Slave module side code                     #
 # ESP8266 WIFI Module                        #
 # 2018                                       #
 ##############################################

                                              //^\\
                                          //^\\ #
    q_/\/\   q_/\/\    q_/\/\   q_/\/\      #   #
 ||||`    /|/|`     <\<\`    |\|\`     #   #
 #*#*#**#**#**#*#*#**#**#**#****#*#**#**#**#*#*#**#**#
 */

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPClient.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#include <TaskScheduler.h>

#include <ArduinoJson.h>

#define PWMRANGE 1023

bool enableDebugging = true;
bool enableSerialBanner = false;

////////////////////////////////////////////////////////
// slave unit vars begin:
// Device type option: fogger, light, nutrientPump, refillPump, drainPump
const String deviceType = "light";
const String deviceId = "2";

const int transistorPin = 14;
int transistorPinPwmValue = 0;
int previousTransistorPinPwmValue = 0;
int tempDeviceIsEnabled = 1;

int deviceState = LOW;
unsigned long previousMillis = 0;
unsigned long dataUpdateInterval = 10000; // in milliseconds
long interval = 1000;

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

String serverAddreess;
String query;
String payload = "-1";

bool httpRequestError = true;
bool updateDataUpdateIntervals = false;
unsigned long previousServerDataUpdateInterval = dataUpdateInterval;
bool lostServerConnection = false;

// Default values
int deviceIsEnabled = 0;
int runOnlyOnce = 1;
int powerPercent = 0;
unsigned long serverDataUpdateInterval = 1;
int serverEnableDeepSleep = 0;
int serverEnableWifiOffWhenNotUsed = 0;
long serverCurrentUnixTime;
unsigned long onDuration = 15;
unsigned long offDuration = 60;
// http request vars end;
////////////////////////////////////////////////////////


////////////////////////////////////////////////////////
// Callback prototypes
void httpRequestCallback();
void httpRequestParserCallback();
void parsedDataToRegularSlaveUnitVariablesCallback();
void updateDataUpdateIntervalsCallback();
void runFoggerCallback();
void runLightCallback();
////////////////////////////////////////////////////////


////////////////////////////////////////////////////////
//Tasks
Task tHttpRequest(dataUpdateInterval, TASK_FOREVER, &httpRequestCallback);

Task tHttpRequestParser(dataUpdateInterval, TASK_FOREVER, &httpRequestParserCallback);

Task tParsedDataToRegularSlaveUnitVariables(dataUpdateInterval, TASK_FOREVER, &parsedDataToRegularSlaveUnitVariablesCallback);

Task tUpdateDataUpdateIntervals(dataUpdateInterval, TASK_FOREVER, &updateDataUpdateIntervalsCallback);

// Fogger runs every seconds, so that it can run the run it on the seconds level (aka. resolution)
Task tRunFogger(1000, TASK_FOREVER, &runFoggerCallback);

Task tRunLight(dataUpdateInterval, TASK_FOREVER, &runLightCallback);

Scheduler taskManager;
////////////////////////////////////////////////////////


////////////////////////////////////////////////////////
////////////////////// FUNCTIONS ///////////////////////
////////////////////////////////////////////////////////
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

        if(enableSerialBanner == true) {
                camelArtAndBanner();
        }

        Serial.println("Booting");

        setupWifi(); // wifi cnk procedure;
        arduinoOTASetup();
        ///////////////////////////////////////////////////////
        ////////////// WIFI - OTA SETUP COMPLETE //////////////
        ///////////////////////////////////////////////////////


        ///////////////////////////////////////////////////////
        ////////////////// SLAVE SETUP BEGIN //////////////////
        ///////////////////////////////////////////////////////

        slaveSetup();

        ///////////////////////////////////////////////////////
        //////////////// SLAVE SETUP COMPLETE /////////////////
        ///////////////////////////////////////////////////////


        ///////////////////////////////////////////////////////
        ////////// TASK MANAGER SETUP PART 1 - BEGIN //////////
        ///////////////////////////////////////////////////////

        setupTaskManagerAddTasks();

        ///////////////////////////////////////////////////////
        /////// TASK MANAGER SETUP PART 1 - COMPLETE //////////
        ///////////////////////////////////////////////////////


        ///////////////////////////////////////////////////////
        ///////// TASK MANAGER SETUP PART 2 - BEGIN ///////////
        ///////////////////////////////////////////////////////

        setupTaskManagerEnableTasks();

        ///////////////////////////////////////////////////////
        /////// TASK MANAGER SETUP PART 2 - COMPLETE //////////
        ///////////////////////////////////////////////////////

        Serial.println(" ");
        Serial.println("Running the system...");
        Serial.println(" ");

        delay(1000);

}

void setupWifi() {
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

void arduinoOTASetup() {
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

                // Disable tasks during the update
                taskManager.disableAll();
        });
        ArduinoOTA.onEnd([]() {
                Serial.println("\nEnd");
        });
        ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {

                // Disable tasks during the update
                taskManager.disableAll();

                Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
        });
        ArduinoOTA.onError([](ota_error_t error) {
                Serial.printf("Error[%u]: ", error);
                if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
                else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
                else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
                else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
                else if (error == OTA_END_ERROR) Serial.println("End Failed");

                // Enable tasks if there is an error, so that system can keep running
                taskManager.enableAll();
        });
        ArduinoOTA.begin();
        // ota setup end;

}

void slaveSetup() {

        // A line break for more readibility
        Serial.println(" ");
        Serial.println("Connecting to server to get slave configuration");
        Serial.println("Please make sure that the following variables are correct for this slave device");
        Serial.print("Device Type: ");
        Serial.println(deviceType);
        Serial.print("Device ID: ");
        Serial.println(deviceId);

        httpRequestCallback();

        httpRequestParserCallback();

        // Need to keep the device out of the inifite reboot loop
        updateDataUpdateIntervals = false;

        parsedDataToRegularSlaveUnitVariablesCallback();

        if(httpRequestError == false) {

                // A line break for more readibility
                Serial.println(" ");

                Serial.print("Updated variables:");
                Serial.println(" ");

                // Output to serial monitor
                Serial.print("deviceIsEnabled:");
                Serial.println(deviceIsEnabled);

                Serial.print("runOnlyOnce:");
                Serial.println(runOnlyOnce);

                Serial.print("powerPercent:");
                Serial.println(powerPercent);

                Serial.print("onDuration (milliseconds):");
                Serial.println(onDuration);

                Serial.print("offDuration (milliseconds):");
                Serial.println(offDuration);

                Serial.print("serverDataUpdateInterval (milliseconds):");
                Serial.println(dataUpdateInterval);

                Serial.print("serverEnableDeepSleep:");
                Serial.println(serverEnableDeepSleep);

                Serial.print("serverEnableWifiOffWhenNotUsed:");
                Serial.println(serverEnableWifiOffWhenNotUsed);

                Serial.print("serverCurrentUnixTime:");
                Serial.println(serverCurrentUnixTime);

                // A line break for more readibility
                Serial.println(" ");

        } else {
                // A line break for more readibility
                Serial.println(" ");

                Serial.println("Couldn't update variables due to the server connection error");

                // A line break for more readibility
                Serial.println(" ");

        }

}

void setupTaskManagerAddTasks() {

        Serial.println(" ");

        taskManager.init();
        Serial.println("Initialized scheduler");

        taskManager.addTask(tHttpRequest);
        tHttpRequest.setInterval(dataUpdateInterval);
        Serial.println("Added 'HTTP request' task");

        taskManager.addTask(tHttpRequestParser);
        tHttpRequestParser.setInterval(dataUpdateInterval);
        Serial.println("Added 'HTTP request parser' task");

        taskManager.addTask(tParsedDataToRegularSlaveUnitVariables);
        tParsedDataToRegularSlaveUnitVariables.setInterval(dataUpdateInterval);
        Serial.println("Added 'Parsed data to regular slave unit variables' task");

        taskManager.addTask(tUpdateDataUpdateIntervals);
        tUpdateDataUpdateIntervals.setInterval(dataUpdateInterval);
        Serial.println("Added 'Update data update intervals' task");

        taskManager.addTask(tRunFogger);
        Serial.println("Added 'Run fogger' task");

        taskManager.addTask(tRunLight);
        tRunLight.setInterval(dataUpdateInterval);
        Serial.println("Added 'Run light' task");

        Serial.println(" ");

}

void setupTaskManagerEnableTasks() {

        Serial.println(" ");

        tHttpRequest.enable();
        Serial.println("Enabled 'HTTP request' task");

        tHttpRequestParser.enable();
        Serial.println("Enabled 'HTTP request parser' task");

        tParsedDataToRegularSlaveUnitVariables.enable();
        Serial.println("Enabled 'Parsed data to regular slave unit variables' task");

        tUpdateDataUpdateIntervals.enable();
        Serial.println("Enabled 'Update data update intervals' task");

        if((deviceType == "fogger") or (deviceType == "FOGGER") or (deviceType == "Fogger")) {

                tRunFogger.enable();
                Serial.println("Enabled 'Run fogger' task");

        } else if ((deviceType == "light") or (deviceType == "LIGHT") or (deviceType == "Light")) {

                tRunLight.enable();
                Serial.println("Enabled 'Run light' task");
        }

}

void camelArtAndBanner() {

        Serial.println(" ");

        Serial.println("##############################################");
        Serial.println("# Team NYUAD, Green / Vaponic Wall Code v1.0 #");
        Serial.println("# Sleve module side code                     #");
        Serial.println("# ESP8266 WIFI Module                        #");
        Serial.println("# 2018                                       #");
        Serial.println("##############################################");

        Serial.println(" ");

        // This looks reqular on serial monitor
        Serial.println("                                              //^\\      ");
        Serial.println("                                          //^\\ #        ");
        Serial.println("     q_/\/\     q_/\/\       q_/\/\  q_/\/\        #   # ");
        Serial.println("       ||||`    /|/|`     <\<\`    |\|\`       #   #     ");
        Serial.println("   #*#*#**#**#**#*#*#**#**#**#****#*#**#**#**#*#*#**#**# ");

        Serial.println(" ");

}

void httpRequestCallback() {

        // wait for WiFi connection
        if ((WiFi.status() == WL_CONNECTED)) {

                HTTPClient http;

                serverAddreess = "http://" + masterHostName + "/" + phpFile;

                http.begin(serverAddreess);

                http.addHeader("Content-Type", "application/x-www-form-urlencoded; charset=UTF-8");

                query = "deviceType=" + deviceType + "&deviceId=" + deviceId + "&deviceIp=" + WiFi.localIP().toString();

                // Send query to the server
                http.POST(query);

                // For debugging
                //http.writeToStream(&Serial);

                // Retrieve the returned result
                payload = http.getString();

                http.end();

                // Change the http request error flag if the result is unsuccessful
                if((payload == " ") or (payload == "-1") or (payload == NULL)) {
                        httpRequestError = true;

                        lostServerConnection = true;

                        // Because there is nothing to parse
                        tHttpRequestParser.disable();
                        tParsedDataToRegularSlaveUnitVariables.disable();

                        // A line break for more readibility
                        Serial.println(" ");
                        Serial.print("Millis(): ");
                        Serial.println(millis());
                        Serial.println("////////////////////////////////");
                        Serial.println("//   LOST SERVER CONNECTION   //");
                        Serial.println("////////////////////////////////");
                        Serial.println(" ");

                } else{

                        if(enableDebugging) {
                                // A line break for more readibility
                                Serial.println(" ");

                                Serial.print("Server addres:");
                                Serial.println(serverAddreess);

                                Serial.print("HTTP Query:");
                                Serial.println(query);

                                Serial.print("HTTP Response: ");
                                Serial.println(payload);

                                // A line break for more readibility
                                Serial.println(" ");
                        }

                        httpRequestError = false;

                        // Parse the payload
                        tHttpRequestParser.enable();

                        if(lostServerConnection == true) {

                                lostServerConnection = false;

                                // A line break for more readibility
                                Serial.println(" ");
                                Serial.print("Millis(): ");
                                Serial.println(millis());
                                Serial.println("////////////////////////////////");
                                Serial.println("// RESTORED SERVER CONNECTION //");
                                Serial.println("////////////////////////////////");
                                Serial.println(" ");
                        }

                }

        } else {
                // A line break for more readibility
                Serial.println(" ");
                Serial.print("Millis(): ");
                Serial.println(millis());
                Serial.println("////////////////////////////////");
                Serial.println("//    LOST WIFI CONNECTION    //");
                Serial.println("////////////////////////////////");
                Serial.println(" ");

                lostServerConnection == true;

                // Because there is nothing to parse
                tHttpRequestParser.disable();
                tParsedDataToRegularSlaveUnitVariables.disable();
        }

}

void httpRequestParserCallback() {

        // If there are no problems with the http result continue, otherwise keep the
        // old variables
        if (httpRequestError == false) {

                // Parsing
                const size_t bufferSize = JSON_OBJECT_SIZE(9) + 200;
                DynamicJsonBuffer jsonBuffer(bufferSize);

                const char* json = "{\"deviceIsEnabled\":1,\"runOnlyOnce\":0,\"powerPercent\":100,\"dataUpdateInterval\":3,\"enableDeepSleep\":0,\"enableWifiOffWhenNotUsed\":0,\"serverCurrentUnixTime\":1533904819,\"onDuration\":5,\"offDuration\":10}";

                JsonObject& root = jsonBuffer.parseObject(payload);

                deviceIsEnabled = root["deviceIsEnabled"]; // 0
                runOnlyOnce = root["runOnlyOnce"]; // 0
                powerPercent = root["powerPercent"]; // 100
                serverDataUpdateInterval = root["dataUpdateInterval"]; // 2
                serverEnableDeepSleep = root["enableDeepSleep"]; // 0
                serverEnableWifiOffWhenNotUsed = root["enableWifiOffWhenNotUsed"]; // 0
                serverCurrentUnixTime = root["serverCurrentUnixTime"];                 // 1533838233

                // Check if they are -1, they should be -1 if the device is a light
                if ((root["onDuration"] > 0 ) and (root["offDuration"] > 0 )) {

                        onDuration = root["onDuration"];

                        // convert seconds to milliseconds for millis()
                        offDuration = root["offDuration"];

                } else {

                        // If there is a problem, revert back to default values

                        onDuration = 15;

                        offDuration = 60;

                }


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

                tParsedDataToRegularSlaveUnitVariables.enable();
        } else {

                if(enableDebugging) {
                        // A line break for more readibility
                        Serial.println(" ");

                        Serial.println("Didn't parsed variables");

                        // A line break for more readibility
                        Serial.println(" ");
                }

                tParsedDataToRegularSlaveUnitVariables.disable();
        }
}

void parsedDataToRegularSlaveUnitVariablesCallback() {

        // If there are no problems with the http result continue, otherwise keep the
        // old variables
        if (httpRequestError == false) {

                ///////////////////////////////////////////////////////
                //////////// VARIABLE CONVERSATION - BEGIN ////////////
                ///////////////////////////////////////////////////////

                // Value mapping to PWM value for the power percent value
                transistorPinPwmValue = map(powerPercent, 0, 100, 0, 1023);

                // if the new value is same, don't update the value
                if((not (dataUpdateInterval == serverDataUpdateInterval*1000)) and (serverDataUpdateInterval > 0 )) {
                        // convert seconds to milliseconds for millis()
                        dataUpdateInterval = serverDataUpdateInterval*1000;

                        updateDataUpdateIntervals = true;

                } else {

                        // In order to prevent any mistakes in the database to cause problems
                        // on the slave unit
                        if(serverDataUpdateInterval == 0 ) {

                                // A line break for more readibility
                                Serial.println(" ");
                                Serial.print("Millis(): ");
                                Serial.println(millis());
                                Serial.println("//////////////////////////////////");
                                Serial.println("// PLEASE CHECK THE DATA UPDATE //");
                                Serial.println("//    INTERVAL VALUE FOR THIS   //");
                                Serial.println("//      SPECIFIC SLAVE UNIT     //");
                                Serial.println("//////////////////////////////////");
                                Serial.println(" ");
                        }

                        updateDataUpdateIntervals = false;

                }

                // convert seconds to milliseconds for millis()
                onDuration = (onDuration)*1000;

                // convert seconds to milliseconds for millis()
                offDuration = (offDuration)*1000;


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

                if(enableDebugging) {

                        Serial.println("Updated ALL VARIABLES successfully");
                }

                // For testing purposes
                //updateTransistorPin();

        } else {

                if(enableDebugging) {
                        // A line break for more readibility
                        Serial.println(" ");

                        Serial.println("Didn't update variables");

                        // A line break for more readibility
                        Serial.println(" ");
                }
        }

}

void updateDataUpdateIntervalsCallback() {

        if(updateDataUpdateIntervals) {
                updateDataUpdateIntervals = false;

                Serial.println("burdayim");

                tHttpRequest.disable();
                tHttpRequestParser.disable();
                tParsedDataToRegularSlaveUnitVariables.disable();
                tUpdateDataUpdateIntervals.disable();


                tHttpRequest.setInterval(dataUpdateInterval);
                tHttpRequestParser.setInterval(dataUpdateInterval);
                tParsedDataToRegularSlaveUnitVariables.setInterval(dataUpdateInterval);
                tUpdateDataUpdateIntervals.setInterval(dataUpdateInterval);

                delay(500);

                tHttpRequest.enable();
                tHttpRequestParser.enable();
                tParsedDataToRegularSlaveUnitVariables.enable();
                tUpdateDataUpdateIntervals.enable();


                if ((deviceType == "light") or (deviceType == "LIGHT") or (deviceType == "Light")) {
                        tRunLight.disable();
                        tRunLight.setInterval(dataUpdateInterval);
                        tRunLight.enable();
                }

                if(enableDebugging) {

                        // A line break for more readibility
                        Serial.println(" ");

                        Serial.println("Updated **DATA UPDATE INTERVALS** successfully");

                        // A line break for more readibility
                        Serial.println(" ");
                }
        }

}

void runFoggerCallback() {

        unsigned long currentMillis = millis();

        if (currentMillis - previousMillis >= interval) {
                // Save the current time to compare "later"
                previousMillis = currentMillis;

                if (deviceState) {
                        // LED is currently on, set time to stay off
                        interval = offDuration;
                } else {
                        // LED is currently off, set time to stay on
                        interval = onDuration;
                }

                // Toggle the LED's state, Fancy, eh!?
                deviceState = !(deviceState);
        }

        updateTransistorPin();

}

void runLightCallback() {

        deviceState = HIGH;

        // In order to prevent direct shut down for lights
        if (powerPercent == 0) {
                powerPercent = 1;
        }

        tempDeviceIsEnabled  = deviceIsEnabled;

        // In order to prevent direct shut down for lights
        if (deviceIsEnabled == 0) {
                deviceIsEnabled = 1;
                transistorPinPwmValue = 1;
        }

        // Slowly adjust the voltage
        if (previousTransistorPinPwmValue < transistorPinPwmValue) {

                int tempPwmValue = transistorPinPwmValue;

                for (int i = previousTransistorPinPwmValue; i <= tempPwmValue; i = i + 10) {

                        transistorPinPwmValue = i;

                        delay(30);

                        updateTransistorPin();
                }

        } else if (previousTransistorPinPwmValue > transistorPinPwmValue) {

                int tempPwmValue = transistorPinPwmValue;

                for (int i = previousTransistorPinPwmValue; i >= tempPwmValue; i = i - 10) {

                        transistorPinPwmValue = i;

                        delay(30);

                        updateTransistorPin();
                }


        } else if (previousTransistorPinPwmValue == transistorPinPwmValue) {

                updateTransistorPin();

        }

        // To prevent current leaks
        if (transistorPinPwmValue > 999 ) {
                transistorPinPwmValue = 1023;
        } else if (transistorPinPwmValue < 26 ) {
                transistorPinPwmValue = 0;
                powerPercent = 0;
        }

        deviceIsEnabled = tempDeviceIsEnabled;

        previousTransistorPinPwmValue = transistorPinPwmValue;

}

void updateTransistorPin() {

        // Simply, if the device shouldn't be turned off, turn off
        if((deviceIsEnabled == 0) or (powerPercent == 0) or (deviceState == LOW)) {

                digitalWrite(transistorPin, LOW);

                if(enableDebugging) {
                        // A line break for more readibility
                        Serial.println(" ");

                        Serial.println("Turned off the device because device is not enabled or power percent is 0");

                        // A line break for more readibility
                        Serial.println(" ");
                }

                // Simply, if the device should be turned on, turn on
        } else if ((deviceIsEnabled == 1) and (powerPercent != 0) and (deviceState == HIGH)) {

                analogWrite(transistorPin, transistorPinPwmValue);

                if(enableDebugging) {
                        // A line break for more readibility
                        Serial.println(" ");

                        Serial.print("Turned on the device. PWM value is ");
                        Serial.println(transistorPinPwmValue);

                        // A line break for more readibility
                        Serial.println(" ");
                }

        }

}

void loop() {

        ArduinoOTA.handle();

        // Run task manager tasks
        taskManager.execute();

}
