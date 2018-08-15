<?php

/*
##############################################
# Team NYUAD, Green / Vaponic Wall Code v1.0 #
# Server side code / Status Page             #
# PHP 7.0                                    #
# 2018                                       #
##############################################

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
}

date_default_timezone_set('Asia/Dubai');

function createRows()
{
    $db = new SQLite3('/var/www/html/greenwall.db', SQLITE3_OPEN_READONLY);

    $query = 'SELECT * FROM devices';

    $results = $db->query($query);

    while ($row = $results->fetchArray()) {
        echo "<tr>";
        echo "<td>"; echo $row["dbId"]; echo "</td>";
        echo "<td>"; echo $row["deviceType"]; echo "</td>";
        echo "<td>"; echo $row["deviceId"]; echo "</td>";
        echo "<td>"; echo $row["powerPercent"]; echo "</td>";
        echo "<td>"; echo $row["dataUpdateInterval"]; echo "</td>";
        echo "<td>"; echo $row["deviceIpAddress"]; echo "</td>";
        echo "<td>"; echo $row["enableDeepSleep"]; echo "</td>";
        echo "<td>"; echo $row["enableWifiOffWhenNotUsed"]; echo "</td>";

        if ((time() - $row["lastCommunicatedOn"]) <= 1*($row["dataUpdateInterval"])){
          echo "<td bgcolor='#00FF00'>"; echo $row["lastCommunicatedOn"];
          echo "&nbsp; &nbsp; &nbsp;    :)   ";
        } else if ((time() - $row["lastCommunicatedOn"]) >= 2*($row["dataUpdateInterval"])){
          echo "<td bgcolor='#FF0000'>"; echo $row["lastCommunicatedOn"];
          echo "&nbsp; &nbsp; &nbsp;    :(   ";
        } else if ((time() - $row["lastCommunicatedOn"]) >= 1*($row["dataUpdateInterval"])){
          echo "<td bgcolor='#FFA500'>"; echo $row["lastCommunicatedOn"];
          echo "&nbsp; &nbsp; &nbsp;    :|   ";
        }
        echo "</td>";
        echo "<td>"; echo $row["onDuration"]; echo "</td>";
        echo "<td>"; echo $row["offDuration"]; echo "</td>";
        echo "<td>"; echo $row["onTime"]; echo "</td>";
        echo "<td>"; echo $row["offTime"]; echo "</td>";
        echo "<td>"; echo $row["extraDimAfterTime"]; echo "</td>";
        echo "</tr>";
    }

    // Free the memory, this in NOT done automatically, while your script is running
    $results->finalize();

    // Close database connect
    $db->close();
}

function isNowInTimePeriod($startTimeString, $endTimeString)
{
    $startTimeString = strval($startTimeString);
    $endTimeString = strval($endTimeString);

    $startTimeStringArray = str_split($startTimeString, 2);
    $startTimeString =  date("H:i", mktime($startTimeString[0], $startTimeString[1]));

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

?>

<!DOCTYPE html>
<html>
<head>
<meta http-equiv="refresh" content="3" >
<style>
table, h2, h4 {
    font-family: arial, sans-serif;
    border-collapse: collapse;
    width: 95%;
}

td, th {
    border: 1px solid #232323;
    text-align: left;
    padding: 4px;
}

tr:nth-child(even) {
    background-color: #dddddd;
}
</style>
</head>
<body>

<div align="center" width="100%">
<h2 align="left">Devices Table</h2>

<h4 align="left"><i>

  Page refreshes every 3 second   |   Current Unix time: <?PHP echo time(); ?>   |   Server IP Address: <?PHP echo $_SERVER['REMOTE_ADDR']; ?>   |   Server Hostname: <?PHP echo gethostname();?>
  <br>

</i></h4>

<table id="myTable">
  <tr>
    <th onclick="sortTable(0)">DB ID</th>
    <th onclick="sortTable(1)">Device Type</th>
    <th onclick="sortTable(2)">Device ID</th>
    <th onclick="sortTable(3)">Power Percent</th>
    <th onclick="sortTable(4)">Data Update Interval (Seconds)</th>
    <th onclick="sortTable(5)">IP Address</th>
    <th onclick="sortTable(6)">Deep Sleep Enabled</th>
    <th onclick="sortTable(7)">WIFI Off Enabled</th>
    <th onclick="sortTable(8)">Last Communicated On (Unix Time)</th>
    <th onclick="sortTable(9)">On Duration (Seconds)</th>
    <th onclick="sortTable(10)">Off Duration (Seconds)</th>
    <th onclick="sortTable(11)">On Time </th>
    <th onclick="sortTable(12)">Off Time </th>
    <th onclick="sortTable(13)">Extra Dim After</th>
  </tr>

  <?PHP createRows(); ?>

</table>

</div>

</body>
<script>
function sortTable(n) {
  var table, rows, switching, i, x, y, shouldSwitch, dir, switchcount = 0;
  table = document.getElementById("myTable");
  switching = true;
  // Set the sorting direction to ascending:
  dir = "asc";
  /* Make a loop that will continue until
  no switching has been done: */
  while (switching) {
    // Start by saying: no switching is done:
    switching = false;
    rows = table.rows;
    /* Loop through all table rows (except the
    first, which contains table headers): */
    for (i = 1; i < (rows.length - 1); i++) {
      // Start by saying there should be no switching:
      shouldSwitch = false;
      /* Get the two elements you want to compare,
      one from current row and one from the next: */
      x = rows[i].getElementsByTagName("TD")[n];
      y = rows[i + 1].getElementsByTagName("TD")[n];
      /* Check if the two rows should switch place,
      based on the direction, asc or desc: */
      if (dir == "asc") {
        if (x.innerHTML.toLowerCase() > y.innerHTML.toLowerCase()) {
          // If so, mark as a switch and break the loop:
          shouldSwitch = true;
          break;
        }
      } else if (dir == "desc") {
        if (x.innerHTML.toLowerCase() < y.innerHTML.toLowerCase()) {
          // If so, mark as a switch and break the loop:
          shouldSwitch = true;
          break;
        }
      }
    }
    if (shouldSwitch) {
      /* If a switch has been marked, make the switch
      and mark that a switch has been done: */
      rows[i].parentNode.insertBefore(rows[i + 1], rows[i]);
      switching = true;
      // Each time a switch is done, increase this count by 1:
      switchcount ++;
    } else {
      /* If no switching has been done AND the direction is "asc",
      set the direction to "desc" and run the while loop again. */
      if (switchcount == 0 && dir == "asc") {
        dir = "desc";
        switching = true;
      }
    }
  }
}
</script>
</html>
