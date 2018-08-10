<?php

// Report errors as http response,
// IT SHOULD BE ENABLED ONLY FOR DEBUGGING PURPOSES
$enableDebugging = false;
if ($enableDebugging) {
    error_reporting(E_ALL);
    ini_set('display_errors', 1);
    ini_set('html_errors', 'On');
}

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
urlParser();

updadeLastCommunicatedTimeOnDb('/var/www/html/greenwall.db', $deviceType, $deviceId, $deviceIp);

// If there are no errors related to getting the type and id of the device,
// keep going. If there is an error, send "-1" as an error indication
if (!$error) {
    $row = getDataFromDb('/var/www/html/greenwall.db', $deviceType, $deviceId);

    if ($row['deviceType'] == "fogger") {
        $response = transferDbInformationToResponse($row, $response);
        //$response['deviceIsEnabled'] = 0;
        $response['runOnlyOnce'] = 0;
        printArrayAsJson($response);
    } elseif ($row['deviceType'] == "light") {
        $response = transferDbInformationToResponse($row, $response);
        $response['deviceIsEnabled'] = 0;
        $response['runOnlyOnce'] = 0;
        printArrayAsJson($response);
    } elseif (($row['deviceType'] == "refillPump") || ($row['deviceType'] == "nutrientPump") || ($row['deviceType'] == "drainPump")) {
        $response = transferDbInformationToResponse($row, $response);
        printArrayAsJson($response);
    }
} else {
    echo '-1';
}

///////////////////////////////////////////////////////////////////
//////////////////////////// FUNCTIONS ////////////////////////////
///////////////////////////////////////////////////////////////////
function urlParser()
{
    //print_r($_GET);
    if (!empty($_GET['deviceType']) && !empty($_GET['deviceId'])) {
        $GLOBALS['deviceType'] = $_GET['deviceType'];
        $GLOBALS['deviceId'] = $_GET['deviceId'];
        $GLOBALS['deviceIp'] = $_GET['deviceIp'];

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

    if ($deviceType == 'sensor') {
        $query = 'SELECT * FROM sensors WHERE sensorType = "'.$deviceType.'" AND sensorId = '.$deviceId;
    } else {
        $query = 'SELECT * FROM devices WHERE deviceType = "'.$deviceType.'" AND deviceId = '.$deviceId;
    }

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

    if ($deviceType == 'sensor') {
        $query = 'UPDATE sensors SET lastCommunicatedOn = '.time().', sensorIpAddress = "'.$deviceIp.'" WHERE sensorType = "'.$deviceType.'" AND sensorId = '.$deviceId;
    } else {
        $query = 'UPDATE devices SET lastCommunicatedOn = '.time().', deviceIpAddress = "'.$deviceIp.'" WHERE deviceType = "'.$deviceType.'" AND deviceId = '.$deviceId;
    }

    //echo $query;

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
            $value = -1;
        }
    }

    return $array;
}


function printArrayAsJson($array)
{
    print_r(json_encode($array));
}
