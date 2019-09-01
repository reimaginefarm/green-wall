/*
 ################################################
 # Team NYUAD, Green / Vaponic Wall Code v1.3   #
 # Slave module side code                       #
 # ESP8266 WIFI Module                          #
 # 2018                                         #
 # Author: woswos                               #
 ################################################

                                              //^\\
                                          //^\\ #
    q_/\/\   q_/\/\    q_/\/\   q_/\/\      #   #
 ||||`    /|/|`     <\<\`    |\|\`     #   #
 #*#*#**#**#**#*#*#**#**#**#****#*#**#**#**#*#*#**#**#
 */

// Default libraries
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPClient.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

// For running functions at intervals
// https://github.com/arkhipenko/TaskScheduler
#include <TaskScheduler.h>

// For parsing the data transferred from the server
// https://github.com/bblanchon/ArduinoJson
#include <ArduinoJson.h>

// Default I2C library
#include <Wire.h>

// Adafruit unified sensors library
#include <Adafruit_Sensor.h>

// For the light sensor
// https://github.com/adafruit/Adafruit_TSL2561
#include <Adafruit_TSL2561_U.h>

// For the pressure, altitute and temperature sensor
// https://github.com/adafruit/Adafruit_BMP085_Unified
#include <Adafruit_BMP085_U.h>

// For the humidity and temperature sensor (temperature is inccuarate)
// https://github.com/adafruit/DHT-sensor-library
#include <DHT.h>
#include <DHT_U.h>

// For the I2C ADC converter
// https://github.com/adafruit/Adafruit_ADS1X15
#include <Adafruit_ADS1015.h>

// For the one-wire interface
// https://github.com/PaulStoffregen/OneWire
#include <OneWire.h>

// For the dallas temperature sensor
// https://github.com/milesburton/Arduino-Temperature-Control-Library
#include <DallasTemperature.h>

// Defining the maximum avaliable pwm value for the pins of ESP8266
#define PWMRANGE 1023

// For splitting strings into substrings
// https://github.com/aharshac/StringSplitter
#include "StringSplitter.h"



/**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//*//**/
/**//* VARIABLES THAT NEEDS TO BE ADJUSTED BASED ON THE SLAVE UNIT CONFIGURATION *//**/
/**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//*//**/
/**/
/**/////////////////////////////////////////////////////////
/**/// debugging vars begin:
/**/ bool enableDebugging = true;
/**/ bool enableSerialBanner = true;
/**/// debugging vars end;
/**/////////////////////////////////////////////////////////
/**/
/**/////////////////////////////////////////////////////////
/**/// slave unit vars begin:
/**/// Device type option: fogger, light, pump, valve,
/**///                     lightSensor, pressureSensor, flowSensor, humiditySensor,
/**///                     phSensor, ecSensor
/**/// You can enter up to ten devices for now
/**/// Please enter the device IDs in the same order that use use for device types
/**/// Buraya cihazlarin yazildigi sira ayrica transistorPin'lere baglanma sirasini
/**///     belirliyor. Yani ilk yazılan transistorPin'e baglanmali ve ikinci yazilan
/**///     ise transistorPin2'ye baglanmis olmali >>>>>>>>> IMPORTANT <<<<<<<<<<
/**/ String slaveUnitId = "1";
/**/
/**/// Transistor control pin
/**/ const int transistorPin = 14;
/**/ const int transistorPin2 = 15;
/**/ const int transistorPin3 = 16;
/**/
/**/// I2C Pins
/**/ const int sdaPin = 5;   // >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> bunları degistirmek simdilik bi ise yaramıyor
/**/ const int sclPin = 4;
/**///const int i2cAddressChangePin = 12;
/**/
/**/// One wire interface pin (for sensors that communicate over only one data)
/**/// Data pin is the digital I/O pin, not the analog one
/**/ int oneWireInterfacePin1 = 13;
/**/ int oneWireInterfacePin2 = 12;
/**/// slave unit vars end;
/**/////////////////////////////////////////////////////////
/**/
/**/////////////////////////////////////////////////////////
/**/// Update these with values suitable for your network.
/**/// cnk vars begin:
/**/ unsigned long currentTime;
/**/ const char* ssid = "2.4g";
/**/ const char* password = "dauyndauyn";
/**/// cnk vars end;
/**/////////////////////////////////////////////////////////
/**/
/**/////////////////////////////////////////////////////////
/**/// http request vars begin:
/**/ String masterHostName = "vaponicwalltestpi";
/**/ String phpFile = "index.php";
/**/ String bootPhpFile = "boot.php";
/**/// http request vars end;
/**/////////////////////////////////////////////////////////
/**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//*//**/
/**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//*//**/



////////////////////////////////////////////////////////
// These are place holders
char* connectedDevices = "fogger, light, lightSensor, pressureSensor, humiditySensor, waterLevelSensor, valve, ecSensor, phSensor";
char* connectedDeviceIds = "1, 1, 1, 1, 1, 1, 1, 1, 1";

// The delimiter used to write device names. It is comma by default
// ' ' should be used, not " " to decorate the delimiter character >>>> IMPORTANT >>>>
char delimiter = ',';

// slave unit vars begin:
String deviceType[] = { " ", " ", " ", " ", " ", " ", " ", " ", " ", " "};
String deviceId[] = { " ", " ", " ", " ", " ", " ", " ", " ", " ", " "};

int totalNumberOfConnectedDevices = 0;
int currentDevice = 0;

// Variables to store data to run the code properly
int indexOfFirstDevice = -1;
int indexOfSecondDevice = -1;
int indexOfThirdDevice = -1;

int transistorPinPwmValue[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int previousTransistorPinPwmValue[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int tempDeviceIsEnabled[] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1};

int deviceState[] = {LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW};
unsigned long previousMillis[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
unsigned long dataUpdateInterval[] = {10000, 10000, 10000, 10000, 10000, 10000, 10000, 10000, 10000, 10000};  // in milliseconds
long interval = 1000;

// Enable either deepsleep or wifi off or none
bool enableDeepSleep[] = {false, false, false, false, false, false, false, false, false, false};
bool enableWifiOffWhenNotUsed[] = {false, false, false, false, false, false, false, false, false, false};

String isSensor[] = { "0", "0", "0", "0", "0", "0", "0", "0", "0", "0"};
String sensorReading[] = { "0", "0", "0", "0", "0", "0", "0", "0", "0", "0"};
bool sensorDataTransferRequest = false;

// slave unit vars end;
////////////////////////////////////////////////////////


////////////////////////////////////////////////////////
// http request vars begin:
String serverAddreess;
String query;
String payload = "-1";

bool httpRequestError = true;
bool updateDataUpdateIntervals = false;
unsigned long previousServerDataUpdateInterval[] = {10000, 10000, 10000, 10000, 10000, 10000, 10000, 10000, 10000, 10000};  // in milliseconds
bool lostServerConnection = false;

// Default values
int deviceIsEnabled[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int runOnlyOnce[] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
int powerPercent[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
unsigned long serverDataUpdateInterval[] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
int serverEnableDeepSleep[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int serverEnableWifiOffWhenNotUsed[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
long serverCurrentUnixTime;
unsigned long onDuration[] = {15, 15, 15, 15, 15, 15, 15, 15, 15, 15};
unsigned long offDuration[] = {60, 60, 60, 60, 60, 60, 60, 60, 60, 60};
// http request vars end;
////////////////////////////////////////////////////////


////////////////////////////////////////////////////////
// Callback prototypes for the task manager
void runFoggerCallback();
void runFoggerDataUpdaterCallback();
void runLightCallback();
void runValveCallback();
void runPumpCallback();
void runSensorCallback();
////////////////////////////////////////////////////////


////////////////////////////////////////////////////////
//Tasks
// Fogger runs every seconds, so that it can run the run it on the seconds level (aka. resolution)
Task tRunFogger(1000, TASK_FOREVER, &runFoggerCallback);

// This task is special to the fogger because the tRunFogger task runs every second
//     to change the state (turned off/on) of the fogger. I could include the data
//     update functionality in it but it doesn't make sense to send queries to server
//     about the state of the foggers. Thus, this task checks the data updates seperately
Task tRunFoggerDataUpdater(1000, TASK_FOREVER, &runFoggerDataUpdaterCallback);

Task tRunLight(1000, TASK_FOREVER, &runLightCallback);

Task tRunValve(1000, TASK_FOREVER, &runValveCallback);

Task tRunPump(1000, TASK_FOREVER, &runPumpCallback);

Task tRunSensor(1000, TASK_FOREVER, &runSensorCallback);

Scheduler taskManager;
////////////////////////////////////////////////////////


////////////////////////////////////////////////////////
////////////////////// FUNCTIONS ///////////////////////
////////////////////////////////////////////////////////

// Setting up the system for the first use. This function executes the other
//      setup functions in the required order.
void setup() {

        pinMode(transistorPin, OUTPUT);
        digitalWrite(transistorPin, LOW);

        pinMode(transistorPin2, OUTPUT);
        digitalWrite(transistorPin2, LOW);

        Serial.begin(115200);

        if(enableSerialBanner == true) {
                camelArtAndBanner();
        }

        Serial.println("Booting");

        setupWifi(); // wifi cnk procedure;

        arduinoOTASetup();

        getConnectedDevicesFromDb();

        deviceTypesAndIdsSetup();

        slaveSetup();

        taskManagerSetup();

        currentDevice = 0;

        Serial.println(" ");
        Serial.print("Millis(): ");
        Serial.println(millis());
        Serial.println("//////////////////////////////////");
        Serial.println("//        SETUP COMPLETED       //");
        Serial.println("//      RUNNING THE SYSTEM      //");
        Serial.println("//////////////////////////////////");
        Serial.println(" ");

        delay(1000);

}

// Sets  up the wifi network and ensures that the devices is connected to the
//      network with the given SSID and password.
void setupWifi() {
        // wifi network cnk begin:
        Serial.println();
        Serial.print("Connecting to ");
        Serial.println(ssid);

        if(enableWifiOffWhenNotUsed) {
                // Disable the WiFi persistence.  The ESP8266 will not load and save WiFi settings in the flash memory.
                WiFi.persistent( false );
        }

        WiFi.hostname("vaponicwall-" + String(deviceType[0]) + "-" + String(deviceId[0]));

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

// Arduino Over-The-Air updates setup.
// This is usefull for uplading new code to the chip without actually connecting
//      the usb cable.
void arduinoOTASetup() {
        // ota setup begin:

        // Port defaults to 8266
        // ArduinoOTA.setPort(8266);

        // Hostname defaults to esp8266-[ChipID]
        String slaveHostName =  "vaponicwall " + deviceType[0] + " " + deviceId[0];

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

// Gets the connected device list and their ids from the server during the setup.
void getConnectedDevicesFromDb(){
        // A line break for more readibility
        Serial.println(" ");
        Serial.println("/////////////////////////////////////////////////////////////////////////////////////");
        Serial.println("Connecting to server to get connected devices to this slave unit");
        Serial.println(" ");

        // wait for WiFi connection
        if ((WiFi.status() == WL_CONNECTED)) {

                HTTPClient http;

                serverAddreess = "http://" + masterHostName + "/" + bootPhpFile;

                http.begin(serverAddreess);

                http.addHeader("Content-Type", "application/x-www-form-urlencoded; charset=UTF-8");

                query = "slaveUnitId=" + slaveUnitId + "&slaveUnitIp=" + WiFi.localIP().toString();

                // For debugging
                //Serial.println(query);

                // Send query to the server
                http.POST(query);

                // For debugging
                //http.writeToStream(&Serial);

                // Retrieve the returned result
                payload = http.getString();

                http.end();

                // Change the http request error flag if the result is unsuccessful
                if((payload == " ") or (payload == "-1") or (payload == NULL) or (payload == "ERROR")) {

                        if(enableDebugging) {
                                // A line break for more readibility
                                Serial.println(" ");

                                Serial.println("/////////////////////////////////////////////////////////////////////////////////////");

                                Serial.print("Server addres:");
                                Serial.println(serverAddreess);

                                Serial.print("HTTP Query:");
                                Serial.println(query);

                                Serial.print("HTTP Response: ");
                                Serial.println(payload);

                                // A line break for more readibility
                                Serial.println(" ");
                        }

                        // A line break for more readibility
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

                        // If there are no problems parse the data retrieved from
                        //      the server.
                        // Parsing

                        StringSplitter *splitter = new StringSplitter(payload, '_', 2);

                        //int itemCount = splitter->getItemCount();

                        String devices = splitter->getItemAtIndex(0);
                        String deviceIds = splitter->getItemAtIndex(1);

                        // Transfer substrings to the actual char arrays
                        devices.toCharArray(connectedDevices, devices.length()+1);
                        deviceIds.toCharArray(connectedDeviceIds, deviceIds.length()+1);

                        Serial.print("Retrieved list of connected devices from server");
                        Serial.println("/////////////////////////////////////////////////////////////////////////////////////");
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

        }


}

// Splits the entered device types and IDs into deviceType[] and deviceId[]
//      arrays.
void deviceTypesAndIdsSetup() {

        totalNumberOfConnectedDevices = countChars(connectedDevices, delimiter) + 1;

        String tempDeviceType = "";
        String tempDeviceId = "";

        for(int i = 0; i < totalNumberOfConnectedDevices; i++) {

                tempDeviceType = getValue(connectedDevices, delimiter, i);
                tempDeviceId = getValue(connectedDeviceIds, delimiter, i);

                // Check the first character of the string
                if((tempDeviceType.charAt(0) == ' ') or (tempDeviceType.charAt(0) == delimiter)) {
                        // Remove 1 characters starting at index=0
                        // Simply remove the space character in the beginning
                        tempDeviceType.remove(0, 1);
                }

                // Check the first character of the string
                if((tempDeviceId.charAt(0) == ' ') or (tempDeviceId.charAt(0) == delimiter)) {
                        // Remove 1 characters starting at index=0
                        // Simply remove the space character in the beginning
                        tempDeviceId.remove(0, 1);
                }

                // Check the last character of the string
                if((tempDeviceType.charAt(tempDeviceType.length() - 1) == ' ') or (tempDeviceType.charAt(tempDeviceType.length() - 1) == delimiter)) {
                        // Remove 1 characters starting at index=0
                        // Simply remove the space character at the end
                        tempDeviceType.remove(tempDeviceType.length() - 1, 1);
                }

                // Check the last character of the string
                if((tempDeviceId.charAt(tempDeviceId.length() - 1) == ' ') or (tempDeviceId.charAt(tempDeviceId.length() - 1) == delimiter)) {
                        // Remove 1 characters starting at index=0
                        // Simply remove the space character at the end
                        tempDeviceId.remove(tempDeviceId.length() - 1, 1);
                }

                // If user entered an extra comma at the end, fix it because it
                //      will indicate more devices than actually there are.
                if ((tempDeviceType == " ") or (tempDeviceType == "")) {
                        totalNumberOfConnectedDevices = totalNumberOfConnectedDevices - 1;
                } else {
                        deviceType[i] = tempDeviceType;
                        deviceId[i] = tempDeviceId;
                }
        }

}

// Gets the initial variables from the server and makes sure that the system is
//      ready for the first run.
void slaveSetup() {

        // A line break for more readibility
        Serial.println(" ");
        Serial.println("/////////////////////////////////////////////////////////////////////////////////////");
        Serial.println("Connecting to server to get slave configuration");
        Serial.println("Please make sure that the following variables are correct for this slave device");
        Serial.println(" ");

        for (int i = 0; i < totalNumberOfConnectedDevices; i++) {
                Serial.print("Device Type: ");
                Serial.println(deviceType[i]);
                Serial.print("Device ID: ");
                Serial.println(deviceId[i]);
                Serial.println(" ");
        }

        Serial.println("/////////////////////////////////////////////////////////////////////////////////////");

        // Read this function's explanation
        assignTransistorPinsToDevices();

        // Loop through all devices
        for (int i = 0; i < totalNumberOfConnectedDevices; i++) {

                Serial.println("/////////////////////////////////////////////////////////////////////////////////////");

                currentDevice = i;

                httpRequestCallback();

                // Need to keep the device out of the inifite reboot loop
                updateDataUpdateIntervals = false;

                if(httpRequestError == false) {

                        // A line break for more readibility
                        Serial.println(" ");

                        Serial.println("//////////////////////////////////");
                        Serial.print("Updated variables for ");
                        Serial.print(deviceType[currentDevice]);
                        Serial.print(" ");
                        Serial.print(deviceId[currentDevice]);
                        Serial.print(": ");
                        Serial.println(" ");

                        // Output to serial monitor
                        Serial.print("deviceIsEnabled:");
                        Serial.println(deviceIsEnabled[currentDevice]);

                        Serial.print("runOnlyOnce:");
                        Serial.println(runOnlyOnce[currentDevice]);

                        Serial.print("powerPercent:");
                        Serial.println(powerPercent[currentDevice]);

                        Serial.print("onDuration (milliseconds):");
                        Serial.println(onDuration[currentDevice]);

                        Serial.print("offDuration (milliseconds):");
                        Serial.println(offDuration[currentDevice]);

                        Serial.print("serverDataUpdateInterval (milliseconds):");
                        Serial.println(dataUpdateInterval[currentDevice]);

                        Serial.print("serverEnableDeepSleep:");
                        Serial.println(serverEnableDeepSleep[currentDevice]);

                        Serial.print("serverEnableWifiOffWhenNotUsed:");
                        Serial.println(serverEnableWifiOffWhenNotUsed[currentDevice]);

                        Serial.print("serverCurrentUnixTime:");
                        Serial.println(serverCurrentUnixTime);

                        Serial.println("//////////////////////////////////");
                        // A line break for more readibility
                        Serial.println(" ");

                } else {
                        // A line break for more readibility
                        Serial.println(" ");

                        Serial.println("///////////////////////////////////////////////////////////////////");
                        Serial.println("// Couldn't update variables due to the server connection error  //");
                        Serial.println("///////////////////////////////////////////////////////////////////");

                        // A line break for more readibility
                        Serial.println(" ");

                }

                Serial.println("/////////////////////////////////////////////////////////////////////////////////////");

        }

}

// Adds and enables the required tasks on the task manager.
void taskManagerSetup() {

        bool alreadySetTheSensor = false;

        taskManager.init();

        Serial.println(" ");
        Serial.println("////////////////////////////////////////////////////////////////////");
        Serial.println("//////////     *Initialized Scheduler (Task Manager)*     //////////");
        Serial.println("////////////////////////////////////////////////////////////////////");

        // Loop through all devices
        for (int i = 0; i < totalNumberOfConnectedDevices; i++) {


                if((deviceType[i] == "fogger") or (deviceType[i] == "FOGGER") or (deviceType[i] == "Fogger")) {

                        Serial.println("////////////////////////////////////////////////////////////////////");

                        isSensor[i] = "0";

                        taskManager.addTask(tRunFogger);
                        Serial.println("//////////            Added 'Run fogger' task             //////////");

                        taskManager.addTask(tRunFoggerDataUpdater);
                        tRunFoggerDataUpdater.setInterval(dataUpdateInterval[i]);
                        Serial.println("//////////      Added 'Run fogger data updater' task      //////////");

                        tRunFogger.enable();
                        Serial.println("//////////           Enabled 'Run fogger' task            //////////");

                        tRunFoggerDataUpdater.enable();
                        Serial.println("//////////     Enabled 'Run fogger data updater' task     //////////");

                } else if ((deviceType[i] == "light") or (deviceType[i] == "LIGHT") or (deviceType[i] == "Light")) {

                        Serial.println("////////////////////////////////////////////////////////////////////");

                        isSensor[i] = "0";

                        taskManager.addTask(tRunLight);
                        tRunLight.setInterval(dataUpdateInterval[i]);

                        Serial.println("//////////            Added 'Run light' task              //////////");

                        tRunLight.enable();

                        Serial.println("//////////           Enabled 'Run light' task             //////////");

                } else if ((deviceType[i] == "valve") or (deviceType[i] == "VALVE") or (deviceType[i] == "Valve")) {

                        Serial.println("////////////////////////////////////////////////////////////////////");

                        isSensor[i] = "0";

                        taskManager.addTask(tRunValve);
                        tRunValve.setInterval(dataUpdateInterval[i]);

                        Serial.println("//////////            Added 'Run valve' task              //////////");

                        tRunValve.enable();

                        Serial.println("//////////           Enabled 'Run valve' task             //////////");

                } else if ((deviceType[i] == "pump") or (deviceType[i] == "PUMP") or (deviceType[i] == "Pump")) {

                        Serial.println("////////////////////////////////////////////////////////////////////");

                        isSensor[i] = "0";

                        taskManager.addTask(tRunPump);
                        tRunPump.setInterval(dataUpdateInterval[i]);

                        Serial.println("//////////            Added 'Run pump' task             //////////");

                        tRunPump.enable();

                        Serial.println("//////////           Enabled 'Run pump' task            //////////");

                } else if ((deviceType[i].indexOf("Sensor") > 0) or (deviceType[i].indexOf("SENSOR") > 0) or (deviceType[i].indexOf("sensor") > 0)) {

                        isSensor[i] = "1";

                        if(alreadySetTheSensor == false) {

                                Serial.println("////////////////////////////////////////////////////////////////////");

                                // Get the shortest data update interval among the sensors and set it as the data update interval for sensors
                                int shortestSensorDataUpdateInterval = getShortestSensorDataUpdateInterval();

                                taskManager.addTask(tRunSensor);
                                tRunSensor.setInterval(shortestSensorDataUpdateInterval);

                                Serial.println("//////////            Added 'Run sensor' task             //////////");

                                tRunSensor.enable();

                                Serial.println("//////////           Enabled 'Run sensor' task            //////////");

                                alreadySetTheSensor = true;
                        }

                }

        }

        Serial.println("////////////////////////////////////////////////////////////////////");

}

// Prints the banner for the code and a camel art to the serial monitor.
// This is here only for fun purposes and it doesn't serve any functionality
//      in the first place. That being said, I like that ASCII camel art :)
void camelArtAndBanner() {

        Serial.println(" ");

        Serial.println("################################################");
        Serial.println("# Team NYUAD, Green / Vaponic Wall Code v1.3   #");
        Serial.println("# Slave module side code                       #");
        Serial.println("# ESP8266 WIFI Module                          #");
        Serial.println("# 2018                                         #");
        Serial.println("# Author: woswos                               #");
        Serial.println("################################################");

        Serial.println(" ");

        // This looks reqular on serial monitor
        Serial.println("                                              //^\\      ");
        Serial.println("                                          //^\\ #        ");
        Serial.println("     q_/\/\     q_/\/\       q_/\/\  q_/\/\        #   # ");
        Serial.println("       ||||`    /|/|`     <\<\`    |\|\`       #   #     ");
        Serial.println("   #*#*#**#**#**#*#*#**#**#**#****#*#**#**#**#*#*#**#**# ");

        Serial.println(" ");

}

// Does the https requests for the code. Connects to the main server and gets
//      the reply in terms of a JSON formatted string.
// This function also tries to handle possible server connection errors to keep
//      the system running with the old information even if there is a problem
//      with the main server.
void httpRequestCallback() {

        // wait for WiFi connection
        if ((WiFi.status() == WL_CONNECTED)) {

                HTTPClient http;

                serverAddreess = "http://" + masterHostName + "/" + phpFile;

                http.begin(serverAddreess);

                http.addHeader("Content-Type", "application/x-www-form-urlencoded; charset=UTF-8");

                String deviceDataTransferRequest = " ";

                // If the device is not a sensor, don't send a reading accidentally
                if (sensorDataTransferRequest) {
                        deviceDataTransferRequest = "1";
                } else {
                        deviceDataTransferRequest = "0";
                        sensorReading[currentDevice] = "0";
                }

                query = "deviceType=" + deviceType[currentDevice] + "&deviceId=" + deviceId[currentDevice] + "&deviceIp=" + WiFi.localIP().toString() + "&slaveUnitId=" + slaveUnitId + "&deviceDataTransferRequest=" + deviceDataTransferRequest + "&dataToTransfer=" + sensorReading[currentDevice];

                // For debugging
                //Serial.println(query);

                // Send query to the server
                http.POST(query);

                // For debugging
                //http.writeToStream(&Serial);

                // Retrieve the returned result
                payload = http.getString();

                http.end();

                // Change the http request error flag if the result is unsuccessful
                if((payload == " ") or (payload == "-1") or (payload == NULL) or (payload == "ERROR")) {
                        httpRequestError = true;

                        lostServerConnection = true;

                        if(enableDebugging) {
                                // A line break for more readibility
                                Serial.println(" ");

                                Serial.println("/////////////////////////////////////////////////////////////////////////////////////");

                                Serial.print("Server addres:");
                                Serial.println(serverAddreess);

                                Serial.print("HTTP Query:");
                                Serial.println(query);

                                Serial.print("HTTP Response: ");
                                Serial.println(payload);

                                // A line break for more readibility
                                Serial.println(" ");
                        }

                        // A line break for more readibility
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


                        if ((sensorDataTransferRequest) and (enableDebugging)) {
                                Serial.println("Sent the sensor value to the server!");

                                // A line break for more readibility
                                Serial.println(" ");
                        }

                        // Reverting back to the original value
                        sensorDataTransferRequest = false;

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


                        // If there are no problems parse the data retrieved from
                        //      the server.
                        httpRequestParserCallback();

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

        }

}

// Converts the string (in JSON format) gathered by the http request to regular
//      arduino variables. So that they can be used in the rest of the code.
void httpRequestParserCallback() {

        // If there are no problems with the http result continue, otherwise keep the
        // old variables
        if (httpRequestError == false) {

                // Parsing
                const size_t bufferSize = JSON_OBJECT_SIZE(9) + 200;
                DynamicJsonBuffer jsonBuffer(bufferSize);

                const char* json = "{\"deviceIsEnabled\":1,\"runOnlyOnce\":0,\"powerPercent\":100,\"dataUpdateInterval\":3,\"enableDeepSleep\":0,\"enableWifiOffWhenNotUsed\":0,\"serverCurrentUnixTime\":1533904819,\"onDuration\":5,\"offDuration\":10}";

                JsonObject& root = jsonBuffer.parseObject(payload);

                deviceIsEnabled[currentDevice] = root["deviceIsEnabled"]; // 0
                runOnlyOnce[currentDevice] = root["runOnlyOnce"]; // 0
                powerPercent[currentDevice] = root["powerPercent"]; // 100
                serverDataUpdateInterval[currentDevice] = root["dataUpdateInterval"]; // 2
                serverEnableDeepSleep[currentDevice] = root["enableDeepSleep"]; // 0
                serverEnableWifiOffWhenNotUsed[currentDevice] = root["enableWifiOffWhenNotUsed"]; // 0
                serverCurrentUnixTime = root["serverCurrentUnixTime"];                 // 1533838233

                // Check if they are -1, they should be -1 if the device is a light
                if ((root["onDuration"] > 0 ) and (root["offDuration"] > 0 )) {

                        onDuration[currentDevice] = root["onDuration"];

                        // convert seconds to milliseconds for millis()
                        offDuration[currentDevice] = root["offDuration"];

                } else {

                        // If there is a problem, revert back to default values
                        onDuration[currentDevice] = 15;
                        offDuration[currentDevice] = 60;
                }


                // For debugging
                if(enableDebugging) {

                        // A line break for more readibility
                        Serial.println(" ");

                        Serial.print("Parsed variables from HTTP response for ");
                        Serial.print(deviceType[currentDevice]);
                        Serial.print(" ");
                        Serial.print(deviceId[currentDevice]);
                        Serial.print(": ");
                        Serial.println(" ");

                        // Output to serial monitor
                        Serial.print("deviceIsEnabled:");
                        Serial.println(deviceIsEnabled[currentDevice]);

                        Serial.print("runOnlyOnce:");
                        Serial.println(runOnlyOnce[currentDevice]);

                        Serial.print("powerPercent:");
                        Serial.println(powerPercent[currentDevice]);

                        Serial.print("onDuration (seconds):");
                        Serial.println(onDuration[currentDevice]);

                        Serial.print("offDuration (seconds):");
                        Serial.println(offDuration[currentDevice]);

                        Serial.print("serverDataUpdateInterval (seconds):");
                        Serial.println(serverDataUpdateInterval[currentDevice]);

                        Serial.print("serverEnableDeepSleep:");
                        Serial.println(serverEnableDeepSleep[currentDevice]);

                        Serial.print("serverEnableWifiOffWhenNotUsed:");
                        Serial.println(serverEnableWifiOffWhenNotUsed[currentDevice]);

                        Serial.print("serverCurrentUnixTime:");
                        Serial.println(serverCurrentUnixTime);

                        // A line break for more readibility
                        Serial.println(" ");
                }

                // If there are no problems, convert the parsed variables
                parsedDataToRegularSlaveUnitVariablesCallback();

        } else {

                if(enableDebugging) {
                        // A line break for more readibility
                        Serial.println(" ");

                        Serial.println("Didn't parsed variables");

                        // A line break for more readibility
                        Serial.println(" ");
                }

        }
}

// Does the conversation between the variables generated with the JSON parsing
//      function parsedDataToRegularSlaveUnitVariablesCallback().
// Convert the data types, checks the min and max values of the variables.
// Basically, it does the housekeeping
void parsedDataToRegularSlaveUnitVariablesCallback() {

        // If there are no problems with the http result continue, otherwise keep the
        // old variables
        if (httpRequestError == false) {

                ///////////////////////////////////////////////////////
                //////////// VARIABLE CONVERSATION - BEGIN ////////////
                ///////////////////////////////////////////////////////

                // Value mapping to PWM value for the power percent value
                transistorPinPwmValue[currentDevice] = map(powerPercent[currentDevice], 0, 100, 0, 1023);

                // if the new value is same, don't update the value
                if((not (dataUpdateInterval[currentDevice] == serverDataUpdateInterval[currentDevice]*1000)) and (serverDataUpdateInterval[currentDevice] > 0 )) {
                        // convert seconds to milliseconds for millis()
                        dataUpdateInterval[currentDevice] = serverDataUpdateInterval[currentDevice]*1000;

                        updateDataUpdateIntervals = true;

                        // bad practice but needed to make it work more stable
                        delay(500);

                } else {

                        // In order to prevent any mistakes in the database to cause problems
                        // on the slave unit
                        if(serverDataUpdateInterval[currentDevice] == 0 ) {

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
                onDuration[currentDevice] = (onDuration[currentDevice])*1000;

                // convert seconds to milliseconds for millis()
                offDuration[currentDevice] = (offDuration[currentDevice])*1000;


                // Convert to boolean variable
                if(serverEnableDeepSleep[currentDevice] == 1) {
                        enableDeepSleep[currentDevice] = true;
                } else if(serverEnableDeepSleep[currentDevice] == 0) {
                        enableDeepSleep[currentDevice] = false;
                }

                // Convert to boolean variable
                if(serverEnableWifiOffWhenNotUsed[currentDevice] == 1) {
                        enableWifiOffWhenNotUsed[currentDevice] = true;
                } else if(serverEnableWifiOffWhenNotUsed[currentDevice] == 0) {
                        enableWifiOffWhenNotUsed[currentDevice] = false;
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

// Updates the task manager intervals when changed in the database.
// This function ensures that the intervals are changed properly without crashing
//      the task manager and the whole system.
void updateDataUpdateIntervalsCallback() {
/*
        if(updateDataUpdateIntervals) {
                updateDataUpdateIntervals = false;

                // not the light sensor, the light device
                if ((deviceType[currentDevice] == "light") or (deviceType[currentDevice] == "LIGHT") or (deviceType[currentDevice] == "Light")) {
                        tRunLight.disable();
                        tRunLight.setInterval(dataUpdateInterval[currentDevice]);
                        tRunLight.enable();

                } else if ((deviceType[currentDevice].indexOf("Valve") > 0) or (deviceType[currentDevice].indexOf("VALVE") > 0) or (deviceType[currentDevice].indexOf("valve") > 0)) {
                        tRunValve.disable();
                        tRunValve.setInterval(dataUpdateInterval[currentDevice]);
                        tRunValve.enable();

                } else if ((deviceType[currentDevice].indexOf("Pump") > 0) or (deviceType[currentDevice].indexOf("PUMP") > 0) or (deviceType[currentDevice].indexOf("pump") > 0)) {
                        tRunPump.disable();
                        tRunPump.setInterval(dataUpdateInterval[currentDevice]);
                        tRunPump.enable();

                } else if ((deviceType[currentDevice].indexOf("Sensor") > 0) or (deviceType[currentDevice].indexOf("SENSOR") > 0) or (deviceType[currentDevice].indexOf("sensor") > 0)) {
                        // All sensors
                        tRunSensor.disable();
                        tRunSensor.setInterval(dataUpdateInterval[currentDevice]);
                        tRunSensor.enable();
                }

                tHttpRequest.disable();
                tHttpRequestParser.disable();
                tParsedDataToRegularSlaveUnitVariables.disable();
                tUpdateDataUpdateIntervals.disable();

                tHttpRequest.setInterval(dataUpdateInterval[currentDevice]);
                tHttpRequestParser.setInterval(dataUpdateInterval[currentDevice]);
                tParsedDataToRegularSlaveUnitVariables.setInterval(dataUpdateInterval[currentDevice]);
                tUpdateDataUpdateIntervals.setInterval(dataUpdateInterval[currentDevice]);

                delay(500);

                tHttpRequest.enable();
                tHttpRequestParser.enable();
                tParsedDataToRegularSlaveUnitVariables.enable();
                tUpdateDataUpdateIntervals.enable();

                // A line break for more readibility
                Serial.println(" ");
                Serial.print("Millis(): ");
                Serial.println(millis());
                Serial.println("//////////////////////////////////");
                Serial.println("//            UPDATED           //");
                Serial.println("//          DATA UPDATE         //");
                Serial.println("//           VARIABLES          //");
                Serial.println("//////////////////////////////////");
                Serial.println(" ");
        }
 */
} //XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX

// Turns the connected fogger(s) ON and OFF based on the onDuration and offDuration
//      gathered from the main server.
// Has a resolution of 1 seconds, which means the function
//      can change state only once per second. This resolution is more than
//      enough for fogger(s) to operate properly.
// Utilizes the updateTransistorPin() to turn on and off the fogger(s)
// Connected to the transistorPin
void runFoggerCallback() {

        setCurrentDeviceTo("fogger");

        unsigned long currentMillis = millis();

        if (currentMillis - previousMillis[currentDevice] >= interval) {
                // Save the current time to compare "later"
                previousMillis[currentDevice] = currentMillis;

                if (deviceState[currentDevice]) {
                        // Fogger is currently on, set time to stay off
                        interval = offDuration[currentDevice];
                } else {
                        // LED is currently off, set time to stay on
                        interval = onDuration[currentDevice];
                }

                // Toggle the LED's state, Fancy, eh!?
                deviceState[currentDevice] = !(deviceState[currentDevice]);
        }

        updateTransistorPin();

}

// This task is special to the fogger because the tRunFogger task runs every second
//     to change the state (turned off/on) of the fogger. I could include the data
//     update functionality in it but it doesn't make sense to send queries to server
//     about the state of the foggers. Thus, this task checks the data updates seperately
void runFoggerDataUpdaterCallback() {

        setCurrentDeviceTo("fogger");

        httpRequestCallback();
}

// Dims tha lights based on the transistorPinPwmValue gathered from the main server.
// This function tries to dim lights smootly by gradually dimming.
// Utilizes the updateTransistorPin() to apply the PWM values to the connected light(s).
// Connected to the transistorPin
void runLightCallback() {

        setCurrentDeviceTo("light");

        isSensor[currentDevice] = "0";

        httpRequestCallback();

        deviceState[currentDevice] = HIGH;

        // In order to prevent direct shut down for lights
        if (powerPercent[currentDevice] == 0) {
                powerPercent[currentDevice] = 1;
        }

        tempDeviceIsEnabled[currentDevice]  = deviceIsEnabled[currentDevice];

        // In order to prevent direct shut down for lights
        if (deviceIsEnabled[currentDevice] == 0) {
                deviceIsEnabled[currentDevice] = 1;
                transistorPinPwmValue[currentDevice] = 1;
        }

        // Slowly adjust the voltage
        if (previousTransistorPinPwmValue[currentDevice] < transistorPinPwmValue[currentDevice]) {

                int tempPwmValue = transistorPinPwmValue[currentDevice];

                for (int i = previousTransistorPinPwmValue[currentDevice]; i <= tempPwmValue; i = i + 10) {

                        transistorPinPwmValue[currentDevice] = i;

                        delay(30);

                        updateTransistorPin();
                }

        } else if (previousTransistorPinPwmValue[currentDevice] > transistorPinPwmValue[currentDevice]) {

                int tempPwmValue = transistorPinPwmValue[currentDevice];

                for (int i = previousTransistorPinPwmValue[currentDevice]; i >= tempPwmValue; i = i - 10) {

                        transistorPinPwmValue[currentDevice] = i;

                        delay(30);

                        updateTransistorPin();
                }


        } else if (previousTransistorPinPwmValue[currentDevice] == transistorPinPwmValue[currentDevice]) {

                // some case that will be implemented in the future
                //updateTransistorPin();

        }

        // To prevent current leaks
        if (transistorPinPwmValue[currentDevice] > 999 ) {
                transistorPinPwmValue[currentDevice] = 1023;
        } else if (transistorPinPwmValue[currentDevice] < 26 ) {
                transistorPinPwmValue[currentDevice] = 0;
                powerPercent[currentDevice] = 0;
        }


        updateTransistorPin();

        deviceIsEnabled[currentDevice] = tempDeviceIsEnabled[currentDevice];

        previousTransistorPinPwmValue[currentDevice] = transistorPinPwmValue[currentDevice];
}

// Connected to the transistorPin
void runValveCallback() {

        setCurrentDeviceTo("valve");

        httpRequestCallback();

        if (deviceIsEnabled[currentDevice] == 0) {
                deviceState[currentDevice] = LOW;
                transistorPinPwmValue[currentDevice] = 0;
        } else if (deviceIsEnabled[currentDevice] == 1) {
                deviceState[currentDevice] = HIGH;
        }

        updateTransistorPin();

}

// Connected to the transistorPin
void runPumpCallback() {

        setCurrentDeviceTo("pump");

        httpRequestCallback();

        if (deviceIsEnabled[currentDevice] == 0) {
                deviceState[currentDevice] = LOW;
                transistorPinPwmValue[currentDevice] = 0;
        } else if (deviceIsEnabled[currentDevice] == 1) {
                deviceState[currentDevice] = HIGH;
        }

        updateTransistorPin();

}

// Selects and runs the right sensor reading function based on the deviceType.
// Triggers httpRequestCallback() to send the reading to the database.
void runSensorCallback() {

        for (int i = 0; i < totalNumberOfConnectedDevices; i++) {

                // If the selected device is a sensor, go ahead!
                if((isSensor[i] == "1") and (deviceIsEnabled[i] == 1 )) {

                        sensorDataTransferRequest = false;

                        if(enableDebugging) {

                                // A line break for more readibility
                                Serial.println(" ");
                                Serial.println("/////////////////////////////////////////////////////////////////////////////////////");


                        }

                        if((deviceType[i].indexOf("light") >= 0) or (deviceType[i].indexOf("LIGHT") >= 0) or (deviceType[i].indexOf("Light") >= 0)) {

                                // sensorReading is a "String", <i> for you information </i>
                                setCurrentDeviceTo("lightSensor");

                                if(enableDebugging) {

                                        Serial.println("Reading the sensor value...");

                                }

                                sensorReading[i] = lightSensorReading();

                                sensorDataTransferRequest = true;

                        } else if((deviceType[i].indexOf("pressure") >= 0) or (deviceType[i].indexOf("PRESSURE") >= 0) or (deviceType[i].indexOf("Pressure") >= 0)) {

                                // sensorReading is a "String", <i> for you information </i>
                                setCurrentDeviceTo("pressureSensor");

                                if(enableDebugging) {

                                        Serial.println("Reading the sensor value...");

                                }

                                sensorReading[i] = pressureSensorReading();

                                sensorDataTransferRequest = true;

                        } else if((deviceType[i].indexOf("flow") >= 0) or (deviceType[i].indexOf("FLOW") >= 0) or (deviceType[i].indexOf("Flow") >= 0)) {

                                // sensorReading is a "String", <i> for you information </i>
                                setCurrentDeviceTo("flowSensor");

                                if(enableDebugging) {

                                        Serial.println("Reading the sensor value...");

                                }

                                sensorReading[i] = flowSensorReading();

                                sensorDataTransferRequest = true;

                        } else if((deviceType[i].indexOf("humidity") >= 0) or (deviceType[i].indexOf("HUMIDITY") >= 0) or (deviceType[i].indexOf("Humidity") >= 0)) {

                                // sensorReading is a "String", <i> for you information </i>
                                setCurrentDeviceTo("humiditySensor");

                                if(enableDebugging) {

                                        Serial.println("Reading the sensor value...");

                                }

                                sensorReading[i] = humiditySensorReading();

                                sensorDataTransferRequest = true;

                        } else if((deviceType[i].indexOf("ph") >= 0) or (deviceType[i].indexOf("PH") >= 0) or (deviceType[i].indexOf("Ph") >= 0)) {

                                // sensorReading is a "String", <i> for you information </i>
                                setCurrentDeviceTo("phSensor");

                                if(enableDebugging) {

                                        Serial.println("Reading the sensor value...");

                                }

                                sensorReading[i] = phSensorReading();

                                sensorDataTransferRequest = true;

                        } else if((deviceType[i].indexOf("ec") >= 0) or (deviceType[i].indexOf("EC") >= 0) or (deviceType[i].indexOf("Ec") >= 0)) {

                                // sensorReading is a "String", <i> for you information </i>
                                setCurrentDeviceTo("ecSensor");

                                if(enableDebugging) {

                                        Serial.println("Reading the sensor value...");

                                }

                                sensorReading[i] = ecSensorReading();

                                sensorDataTransferRequest = true;

                        } else if((deviceType[i].indexOf("water") >= 0) or (deviceType[i].indexOf("WATER") >= 0) or (deviceType[i].indexOf("Water") >= 0)) {

                                // sensorReading is a "String", <i> for you information </i>
                                setCurrentDeviceTo("waterLevelSensor");

                                if(enableDebugging) {

                                        Serial.println("Reading the sensor value...");

                                }

                                sensorReading[i] = waterLevelSensorReading();

                                sensorDataTransferRequest = true;

                        } else {

                                sensorDataTransferRequest = false;

                                if(enableDebugging) {

                                        // A line break for more readibility
                                        Serial.println(" ");

                                        Serial.println("I am not a sensor, how did I end up executing the sensor function?");

                                        // A line break for more readibility
                                        Serial.println(" ");
                                }

                        }

                        if(sensorDataTransferRequest) {

                                if(enableDebugging) {
                                        Serial.println("Now sending to the server.");
                                }

                                httpRequestCallback();

                                sensorDataTransferRequest = false;

                                if(enableDebugging) {
                                        Serial.println("/////////////////////////////////////////////////////////////////////////////////////");
                                        Serial.println(" ");
                                }

                        }
                }

        }
}

// Turns ON or OFF the transistorPin based on the transistorPinPwmValue variable
void updateTransistorPin() {

        // Simply, if the device shouldn't be turned off, turn off
        if((deviceIsEnabled[currentDevice] == 0) or (powerPercent[currentDevice] == 0) or (deviceState[currentDevice] == LOW)) {

                digitalWrite(getTransistorPin(currentDevice), LOW);

                if(enableDebugging) {
                        // A line break for more readibility
                        Serial.println(" ");

                        Serial.println("Turned off the device because device is not enabled or power percent is 0");

                        // A line break for more readibility
                        Serial.println(" ");
                }

                // Simply, if the device should be turned on, turn on
        } else if ((deviceIsEnabled[currentDevice] == 1) and (powerPercent[currentDevice] != 0) and (deviceState[currentDevice] == HIGH)) {

                analogWrite(getTransistorPin(currentDevice), transistorPinPwmValue[currentDevice]);

                if(enableDebugging) {
                        // A line break for more readibility
                        Serial.println(" ");

                        Serial.print("Turned on the device. PWM value is ");
                        Serial.println(transistorPinPwmValue[currentDevice]);

                        // A line break for more readibility
                        Serial.println(" ");
                }

        }

}

void setCurrentDeviceTo(String deviceTypeName) {

        // Find the index of the given device type name in the device type array
        for(int i = 0; i < totalNumberOfConnectedDevices; i++) {
                if(deviceType[i] == deviceTypeName) {
                        // If the matching device is found, set current device
                        //      to that index
                        currentDevice = i;

                        break;
                }

        }

        if(enableDebugging) {
                // A line break for more readibility
                Serial.println(" ");

                Serial.print("Current device is set to ");

                Serial.print(deviceType[currentDevice]);

                Serial.print(" ");

                Serial.println(deviceId[currentDevice]);

                // A line break for more readibility
                Serial.println(" ");
        }
}

// Print all connected devices with their ids, names, and sensor classification
void printDevices() {

        for (int i = 0; i < totalNumberOfConnectedDevices; i++) {
                Serial.print("Device Type: ");
                Serial.println(deviceType[i]);
                Serial.print("Device ID: ");
                Serial.println(deviceId[i]);
                Serial.print("isSensor: ");
                Serial.println(isSensor[i]);
                Serial.println(" ");
        }

}

// Conected to I2C
String lightSensorReading() {

        Adafruit_TSL2561_Unified tsl = Adafruit_TSL2561_Unified(TSL2561_ADDR_FLOAT, 12345);

        tsl.begin();

        tsl.enableAutoRange(true);

        tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_402MS);

        sensors_event_t event;

        tsl.getEvent(&event);

        // Convert the reading to string, because it can handle every kind of data,
        // even if the sensor is changed in the future
        String reading = String(event.light);

        return reading;
}

// Conected to I2C
String pressureSensorReading() {

        String reading = "";

        Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085);

        bmp.begin();

        sensors_event_t event;
        bmp.getEvent(&event);

        float temperature;
        bmp.getTemperature(&temperature);

        float seaLevelPressure = SENSORS_PRESSURE_SEALEVELHPA;

        // Pressure + Temperature + Altitude
        reading = String(event.pressure) + "_" + String(temperature) + "_" + bmp.pressureToAltitude(seaLevelPressure, event.pressure);

        return reading;
}

// THIS FUNCTION IS NOT READY
String flowSensorReading() {

        String reading = "";

        reading = "sik";


        return reading;
}

// Connected to oneWireInterfacePin1
String HTU210humiditySensorReading() {

        String reading = "";

        #define DHTPIN  oneWireInterfacePin1

        // Uncomment the type of sensor in use:
        #define DHTTYPE           DHT11     // DHT 11
        //#define DHTTYPE           DHT22     // DHT 22 (AM2302)
        //#define DHTTYPE           DHT21     // DHT 21 (AM2301)

        DHT_Unified dht(DHTPIN, DHTTYPE);

        // Initialize device.
        dht.begin();

        // Get temperature event and print its value.
        sensors_event_t event;
        dht.temperature().getEvent(&event);

        String temp = String(event.temperature);

        // Get humidity event and print its value.
        dht.humidity().getEvent(&event);

        String hum = String(event.relative_humidity);

        // Humidity + temperature
        reading = hum + "_" + temp;

        return reading;
}

// Connected to oneWireInterfacePin1
String humiditySensorReading() {

        String reading = "";


        // Humidity + temperature
        reading = hum + "_" + temp;

        return reading;
}

// Connected to ADC to I2C converter
String phSensorReading() {

        int sensorPinOnADC = 0;

        String reading = "";

        // Based on https://www.dfrobot.com/wiki/index.php/PH_meter(SKU:_SEN0161)

        //Store the average value of the sensor feedback
        unsigned long int avgValue;
        float b;
        int buf[10],temp;

        //Get 10 sample value from the sensor for smooth the value
        for(int i=0; i<10; i++) {
                buf[i] = getI2cAdcConverterValue(sensorPinOnADC);
                delay(50);
        }

        //sort the analog from small to large
        for(int i = 0; i < 9; i++) {
                for(int j = i + 1; j < 10; j++) {
                        if(buf[i] > buf[j]) {
                                temp = buf[i];
                                buf[i] = buf[j];
                                buf[j] = temp;
                        }
                }
        }

        avgValue = 0;

        //take the average value of 6 center sample
        for(int i = 2; i < 8; i++) {
                avgValue += buf[i];
        }

        //convert the analog into millivolt
        float phValue = (float)avgValue * 5.0 / 1023.0 / 6.0;

        //convert the millivolt into pH value
        phValue = 3.5 * phValue;

        reading = String(phValue);

        return reading;
}

// Connected to ADC to I2C converter
String ecSensorReading() {

        int sensorPinOnADC = 0;

        String reading = "";

        // Based on https://www.dfrobot.com/wiki/index.php/Analog_EC_Meter_SKU:DFR0300

        //Store the average value of the sensor feedback
        unsigned long int avgValue;
        float b;
        int buf[10],temp;

        float temperature,ECcurrent;

        // Get the temperature of the liquid
        temperature = (dallasTemperatureSensor().toFloat());

        //Get 10 sample value from the sensor for smooth the value
        for(int i=0; i<10; i++) {
                buf[i] = getI2cAdcConverterValue(sensorPinOnADC);
                delay(50);
        }

        //sort the analog from small to large
        for(int i = 0; i < 9; i++) {
                for(int j = i + 1; j < 10; j++) {
                        if(buf[i] > buf[j]) {
                                temp = buf[i];
                                buf[i] = buf[j];
                                buf[j] = temp;
                        }
                }
        }

        avgValue = 0;

        //take the average value of 6 center sample
        for(int i = 2; i < 8; i++) {
                avgValue += buf[i];
        }

        //convert the analog into millivolt
        float averageVoltage = (float)avgValue * 5000 / 1023.0 / 6.0;

        //temperature compensation formula: fFinalResult(25^C) = fFinalResult(current)/(1.0+0.0185*(fTP-25.0));
        float TempCoefficient = 1.0 + 0.0185 * (temperature - 25.0);

        float CoefficientVolatge=(float)averageVoltage/TempCoefficient;

        if(CoefficientVolatge <= 448) {
                //1ms/cm<EC<=3ms/cm
                ECcurrent = 6.84 * CoefficientVolatge - 64.32;

        } else if(CoefficientVolatge <= 1457) {
                //3ms/cm<EC<=10ms/cm
                ECcurrent = 6.98 * CoefficientVolatge - 127;

        } else {
                //10ms/cm<EC<20ms/cm
                ECcurrent = 5.3 * CoefficientVolatge + 2278;
        }

        //convert us/cm to ms/cm
        ECcurrent /= 1000;

        reading = String(ECcurrent) + "_" + String(temperature);

        return reading;
}

// Connected to ADC to I2C converter
String waterLevelSensorReading() {

        int sensorPinOnADC = 0;

        String reading = "";

        //Store the average value of the sensor feedback
        unsigned long int avgValue;
        float b;
        int buf[10],temp;

        //Get 10 sample value from the sensor for smooth the value
        for(int i=0; i<10; i++) {
                buf[i] = getI2cAdcConverterValue(sensorPinOnADC);
                delay(50);
        }

        //sort the analog from small to large
        for(int i = 0; i < 9; i++) {
                for(int j = i + 1; j < 10; j++) {
                        if(buf[i] > buf[j]) {
                                temp = buf[i];
                                buf[i] = buf[j];
                                buf[j] = temp;
                        }
                }
        }

        avgValue = 0;

        //take the average value of 6 center sample
        for(int i = 2; i < 8; i++) {
                avgValue += buf[i];
        }

        //convert the analog into millivolt
        float voltage = (float)avgValue * 5.0 / 1023.0 / 6.0;

        reading = String(voltage);

        return reading;
}

// Connected to oneWireInterfacePin1
String dallasTemperatureSensor() {

        String reading = "";

        // Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
        OneWire oneWire(oneWireInterfacePin1);

        // Pass our oneWire reference to Dallas Temperature.
        DallasTemperature sensors(&oneWire);
        // Start up the library
        sensors.begin();

        // call sensors.requestTemperatures() to issue a global temperature
        // request to all devices on the bus
        sensors.requestTemperatures(); // Send the command to get temperatures

        // After we got the temperatures, we can print them here.
        // We use the function ByIndex, and as an example get the temperature from the first sensor only.

        reading = String(sensors.getTempCByIndex(0));

        return reading;

}

// Reads the value ADC value at the given pin and converts to the 0 - 1023 range
//      for the compability with current libraries that are based on the arduino
//      analog input, which gives values between 0 and 1023 instead of 0 and 32767.
int getI2cAdcConverterValue(int x){

        Adafruit_ADS1115 ads; /* Use this for the 16-bit version */
        //Adafruit_ADS1015 ads;     /* Use thi for the 12-bit version */

        ads.begin();

        int16_t adc;

        adc = ads.readADC_SingleEnded(x);

        // Converting the reading to unsigned 10 bit value
        adc = map(adc, -27100, 27100, -1023, 1023);

        //    adc = map(adc, -1023, 1023, 0, 1023);

        // Making sure that values are in the bounds
        if(adc < 0) {
                adc = 0;
        }

        if(adc > 1023) {
                adc = 1023;
        }

        /*
           float voltage = adc * (5.0 / 1023.0);
           // write the voltage value to the serial monitor:
           Serial.print("voltage: ");
           Serial.println(voltage);
         */

        return adc;
}

int countChars(const char* string, char ch) {
        int count = 0;
        for(; *string; count += (*string++ == ch));
        return count;
}

// Decide which device is the first and which device is the second
void assignTransistorPinsToDevices() {

        for (int i = 0; i < totalNumberOfConnectedDevices; i++) {

                if ((deviceType[i] == "fogger") or (deviceType[i] == "light") or (deviceType[i] == "pump") or (deviceType[i] == "valve")) {

                        if(indexOfFirstDevice == -1) {
                                indexOfFirstDevice = i;
                        } else if(indexOfSecondDevice == -1) {
                                indexOfSecondDevice = i;
                        } else if(indexOfThirdDevice == -1) {
                                indexOfThirdDevice = i;
                        }
                }
        }
}

int getTransistorPin(int givenDeviceId) {

        int assignedDevicePin = transistorPin;

        if(givenDeviceId == indexOfFirstDevice) {
                assignedDevicePin = transistorPin;
        } else if(givenDeviceId == indexOfSecondDevice) {
                assignedDevicePin = transistorPin2;
        }else if(givenDeviceId == indexOfThirdDevice) {
                assignedDevicePin = transistorPin3;
        }

        return assignedDevicePin;
}

// Get the shortest dataUpdateInterval time among all connected devices
int getShortestDataUpdateInterval() {

        int minValue = 0;

        minValue = dataUpdateInterval[0];

        for (int i = 0; i < totalNumberOfConnectedDevices; i++) {
                if (dataUpdateInterval[i] < minValue) {
                        minValue = dataUpdateInterval[i];
                }
        }

        return minValue;
}

// Get the shortest dataUpdateInterval time among the sensors
int getShortestSensorDataUpdateInterval() {

        // Get a reallllyyyy big integer to compare
        int minValue = 999999999999999999999999999999999;

        for (int i = 0; i < totalNumberOfConnectedDevices; i++) {
                if ((dataUpdateInterval[i] < minValue) and (isSensor[i] == "1")) {
                        minValue = dataUpdateInterval[i];
                }
        }

        return minValue;

}

// Splits char array by the delimiter chara
String getValue(String data, char separator, int index) {
        int found = 0;
        int strIndex[] = { 0, -1 };
        int maxIndex = data.length() - 1;

        for (int i = 0; i <= maxIndex && found <= index; i++) {
                if (data.charAt(i) == separator || i == maxIndex) {
                        found++;
                        strIndex[0] = strIndex[1] + 1;
                        strIndex[1] = (i == maxIndex) ? i+1 : i;
                }
        }
        return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

// Main program loop
void loop() {

        ArduinoOTA.handle();

        // Run task manager tasks
        taskManager.execute();

}


//********** THE END **********//
//***** That's all Folks! *****//
// But first, one last ASCII art :)
/*
          __________________________
         / _____________________  /|
        / / ___________________/ / |
       / / /| |               / /  |
      / / / | |              / / . |
     / / /| | |             / / /| |
    / / / | | |            / / / | |
   / / /  | | |           / / /| | |
  / /_/___| | |__________/ / / | | |
 /________| | |___________/ /  | | |
 | _______| | |__________ | |  | | |
 | | |    | | |_________| | |__| | |
 | | |    | |___________| | |____| |
 | | |   / / ___________| | |_  / /
 | | |  / / /           | | |/ / /
 | | | / / /            | | | / /
 | | |/ / /             | | |/ /
 | | | / /              | | ' /
 | | |/_/_______________| |  /
 | |____________________| | /
 |________________________|/

 */
