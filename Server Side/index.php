<?php

/*
################################################
# Team NYUAD, Green / Vaponic Wall Code v1.3   #
# Server side code                             #
# PHP 7.0                                      #
# 2018                                         #
# Author: woswos                               #
################################################

                                              //^\\
                                          //^\\ #
    q_/\/\   q_/\/\    q_/\/\   q_/\/\      #   #
      ||||`    /|/|`     <\<\`    |\|\`     #   #
#*#*#**#**#**#*#*#**#**#**#****#*#**#**#**#*#*#**#**#
*/

// Report errors as http response,
// IT SHOULD BE ENABLED ONLY FOR DEBUGGING PURPOSES
$enableDebugging = false;
if ($enableDebugging) {
    error_reporting(E_ALL);
    ini_set('display_errors', 1);
    ini_set('html_errors', 'On');
} else {
    error_reporting(0);
    ini_set('display_errors', 0);
    ini_set('html_errors', 'Off');
}


date_default_timezone_set('Asia/Dubai');

// The final response that will be returned
// These are the default values
$response = [
    "deviceIsEnabled" => 0, // 0 - false, 1 - true
    "runOnlyOnce" => 1, // 0 - false, 1 - true
    "powerPercent" => 100,
    "dataUpdateInterval" => 2, // in seconds
    "enableDeepSleep" => 0, // 0 - false, 1 - true
    "enableWifiOffWhenNotUsed" => 0, // 0 - false, 1 - true
    "serverCurrentUnixTime" => time(),
    "onDuration" => 0,  // in seconds
    "offDuration" => 0  // in seconds
];

// Parse the variables that are transferred over http
urlParser($enableDebugging);

// If there are no errors related to getting the type and id of the device,
// keep going. If there is an error, send "-1" as an error indication

if (!$error) {
    updadeLastCommunicatedTimeOnDb('/var/www/html/greenwall.db', $deviceType, $deviceId, $deviceIp);

    // If device needs to transfer data to server, do it first
    if ($deviceDataTransferRequest == 1) {
        sendDataToDb('/var/www/html/greenwall.db', $deviceType, $deviceId, $dataToTransfer);
    }

    // Get the device information
    $row = getDataFromDb('/var/www/html/greenwall.db', $deviceType, $deviceId);

    $response = transferDbInformationToResponse($row, $response);
    //////// FOGGER
    if ($row['deviceType'] == "fogger") {
        $response['runOnlyOnce'] = 0;

        printArrayAsJson($response);
    //////// LIGHT
    } elseif ($row['deviceType'] == "light") {
        $response['runOnlyOnce'] = 0;

        // Device timing check
        if (isNowInTimePeriod($row['onTime'], $row['offTime'])) {
            $response['deviceIsEnabled'] = 1;

            // Check if extra dim needs to be activated,
            // 0600 is 6 AM in the morning. So it check between these two times
            if (isNowInTimePeriod($row['extraDimAfterTime'], "0600")) {
                $response['powerPercent'] = $response['powerPercent'] * 0.30;
            }
        } else {
            $response['deviceIsEnabled'] = 0;
        }

        $response['runOnlyOnce'] = 0;

        printArrayAsJson($response);
    //////// PUMP
    } elseif ($row['deviceType'] == "pump") {

        ///// Sensor
        printArrayAsJson($response);
    } elseif ($row['deviceType'] == "lightSensor") {
        ///// Sensor
        printArrayAsJson($response);
    } elseif ($row['deviceType'] == "pressureSensor") {
        ///// Sensor
        printArrayAsJson($response);
    } elseif ($row['deviceType'] == "flowSensor") {
        ///// Sensor
        printArrayAsJson($response);
    } elseif ($row['deviceType'] == "humiditySensor") {
        ///// Sensor
        printArrayAsJson($response);
    } elseif ($row['deviceType'] == "phSensor") {
        ///// Sensor
        printArrayAsJson($response);
    } elseif ($row['deviceType'] == "ecSensor") {
        ///// Sensor
        printArrayAsJson($response);
    } else {
        echo 'ERROR';
    }
} else {
    echo 'ERROR';
    //echo "<br> The time is " . date("H:i");
}

///////////////////////////////////////////////////////////////////
//////////////////////////// FUNCTIONS ////////////////////////////
///////////////////////////////////////////////////////////////////
function urlParser($enableDebugging)
{
    //print_r($_GET);
    if (!empty($_POST['deviceType']) && !empty($_POST['deviceId'])) {
        $GLOBALS['deviceType'] = $_POST['deviceType'];
        $GLOBALS['deviceId'] = $_POST['deviceId'];
        $GLOBALS['deviceIp'] = $_POST['deviceIp'];

        $GLOBALS['deviceDataTransferRequest'] = $_POST['deviceDataTransferRequest'];
        $GLOBALS['dataToTransfer'] = $_POST['dataToTransfer'];

        $GLOBALS['error'] = false;
    } else {
        $GLOBALS['error'] = true;

        // Send this as an error message to the
        if ($enableDebugging) {
            echo 'Please add the deviceType and/or deviceId information to the configuration file\n';
        }
    }
}

function getDataFromDb($dbName, $deviceType, $deviceId)
{
    $db = new SQLite3($dbName, SQLITE3_OPEN_READWRITE);

    $query = 'SELECT * FROM devices WHERE deviceType = "'.$deviceType.'" AND deviceId = '.$deviceId;

    //echo $query;

    $results = $db->query($query);
    $row = $results->fetchArray();

    /*
    echo "<pre>";
    print_r($row);
    echo "</pre>";
    */

    // Free the memory, this in NOT done automatically, while your script is running
    $results->finalize();

    // Close database connect
    $db->close();

    return $row;
}

function updadeLastCommunicatedTimeOnDb($dbName, $deviceType, $deviceId, $deviceIp)
{
    $db = new SQLite3($dbName, SQLITE3_OPEN_READWRITE);

    $query = 'UPDATE devices SET lastCommunicatedOn = '.time().', deviceIpAddress = "'.$deviceIp.'" WHERE deviceType = "'.$deviceType.'" AND deviceId = '.$deviceId;

    //echo $query;

    $db->busyTimeout(5000);

    $db->exec($query);

    // Close database connect
    $db->close();
}

function sendDataToDb($dbName, $deviceType, $deviceId, $deviceData)
{

    // If there are more than one reading that comes from the sensor, divide it
    $splittedDeviceData = explode("_", $deviceData);

    if (count($splittedDeviceData) == 1) {
        $query = 'INSERT INTO deviceLogs (deviceType, deviceId, dateTime, reading1) VALUES ("'.$deviceType.'", "'.$deviceId.'", '.time().', "'.$splittedDeviceData[0].'")';
    } elseif (count($splittedDeviceData) == 2) {
        $query = 'INSERT INTO deviceLogs (deviceType, deviceId, dateTime, reading1, reading2) VALUES ("'.$deviceType.'", "'.$deviceId.'", '.time().', "'.$splittedDeviceData[0].'", "'.$splittedDeviceData[1].'")';
    } elseif (count($splittedDeviceData) == 3) {
        $query = 'INSERT INTO deviceLogs (deviceType, deviceId, dateTime, reading1, reading2, reading3) VALUES ("'.$deviceType.'", "'.$deviceId.'", '.time().', "'.$splittedDeviceData[0].'", "'.$splittedDeviceData[1].'", "'.$splittedDeviceData[2].'")';
    }


    //echo $query;

    $db = new SQLite3($dbName, SQLITE3_OPEN_READWRITE);

    $db->busyTimeout(5000);

    $db->exec($query);

    // Close database connect
    $db->close();
}

function transferDbInformationToResponse($row, $response)
{
    $response['deviceIsEnabled'] = 1;
    $response['runOnlyOnce'] = 1;
    $response['powerPercent'] = $row['powerPercent'];
    $response['dataUpdateInterval'] = $row['dataUpdateInterval'];
    $response['enableDeepSleep'] = $row['enableDeepSleep'];
    $response['enableWifiOffWhenNotUsed'] = $row['enableWifiOffWhenNotUsed'];
    $response['onDuration'] = $row['onDuration'];
    $response['offDuration'] = $row['offDuration'];
    $response = arrayCleaner($response);

    return $response;
}

function arrayCleaner($array)
{

  // Variables that are -1 mean that don't exist for the specific connected device
    foreach ($array as &$value) {
        if (is_null($value)) {
            $value = 0;
        }
    }

    return $array;
}

function isNowInTimePeriod($startTimeString, $endTimeString)
{
    $startTimeString = strval($startTimeString);
    $endTimeString = strval($endTimeString);

    $startTimeStringArray = str_split($startTimeString, 2);
    $startTimeString =  date("H:i", mktime($startTimeStringArray[0], $startTimeStringArray[1]));

    $endTimeStringArray = str_split($endTimeString, 2);
    $endTimeString =  date("H:i", mktime($endTimeStringArray[0], $endTimeStringArray[1]));

    $currentTimeString = date('H:i');
    //$currentTimeStringArray = str_split($currentTimeString, 2);
    //$currentTimeString =  date("H:i", mktime($currentTimeStringArray[0], $currentTimeStringArray[1]));

    $currentTime = DateTime::createFromFormat('H:i', $currentTimeString);
    $startTime = DateTime::createFromFormat('H:i', $startTimeString);
    $endTime = DateTime::createFromFormat('H:i', $endTimeString);

    if ($startTime < $endTime) {
        if (($currentTime >= $startTime) && ($currentTime <= $endTime)) {
            return true;
        } else {
            return false;
        }
    } else {
        if (($currentTime >= $startTime) || ($currentTime <= $endTime)) {
            return true;
        } else {
            return false;
        }
    }
}

function printArrayAsJson($array)
{
    print_r(json_encode($array));
}
