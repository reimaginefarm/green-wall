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

function createRowsForDevices()
{
    $db = new SQLite3('/var/www/html/greenwall.db', SQLITE3_OPEN_READONLY);

    $query = 'SELECT * FROM devices';

    $results = $db->query($query);

    while ($row = $results->fetchArray()) {
        echo "<tr>";
        echo "<td>";
        echo $row["dbId"];
        echo "</td>";
        echo "<td>";
        echo $row["deviceType"];
        echo "</td>";
        echo "<td>";
        echo $row["deviceId"];
        echo "</td>";
        echo "<td>";
        echo $row["powerPercent"];
        echo "</td>";
        echo "<td>";
        echo $row["dataUpdateInterval"];
        echo "</td>";
        echo "<td>";
        echo $row["deviceIpAddress"];
        echo "</td>";
        echo "<td>";
        echo $row["enableDeepSleep"];
        echo "</td>";
        echo "<td>";
        echo $row["enableWifiOffWhenNotUsed"];
        echo "</td>";

        if ((time() - $row["lastCommunicatedOn"]) <= 1*($row["dataUpdateInterval"])) {
            echo "<td bgcolor='#00FF00'>";
            echo $row["lastCommunicatedOn"];
            echo " (";
            echo(time() - $row["lastCommunicatedOn"]);
            echo ")";
            echo "<br>    :)   ";
        } elseif ((time() - $row["lastCommunicatedOn"]) >= 2*($row["dataUpdateInterval"])) {
            echo "<td bgcolor='#FF0000'>";
            echo $row["lastCommunicatedOn"];
            echo " (";
            echo(time() - $row["lastCommunicatedOn"]);
            echo ")";
            echo "<br>    :(   ";
        } elseif ((time() - $row["lastCommunicatedOn"]) >= 1*($row["dataUpdateInterval"])) {
            echo "<td bgcolor='#FFA500'>";
            echo $row["lastCommunicatedOn"];
            echo " (";
            echo(time() - $row["lastCommunicatedOn"]);
            echo ")";
            echo "<br>    :|   ";
        }
        echo "</td>";
        echo "<td>";
        echo $row["onDuration"];
        echo "</td>";
        echo "<td>";
        echo $row["offDuration"];
        echo "</td>";
        echo "<td>";
        echo $row["onTime"];
        echo "</td>";
        echo "<td>";
        echo $row["offTime"];
        echo "</td>";
        echo "<td>";
        echo $row["extraDimAfterTime"];
        echo "</td>";
        echo "<td>";
        echo $row["pumpMode"];
        echo "</td>";
        echo "</tr>";
    }

    // Free the memory, this in NOT done automatically, while your script is running
    $results->finalize();

    // Close database connect
    $db->close();
}


function createRowsForColumns()
{
    $db = new SQLite3('/var/www/html/greenwall.db', SQLITE3_OPEN_READONLY);

    $query = 'SELECT * FROM columns';

    $results = $db->query($query);
    $columnCount = 0;


    while ($column = $results->fetchArray()) {
        echo "<td style='border-collapse: collapse; border: none;'>";
        echo "<table>";
        echo "<tr>";
        echo "<th>";
        echo "<i>Column: ";
        echo $column["columnId"];
        echo " (";
        echo $column["plantType"];
        echo ") </i>";
        echo "</th>";
        echo "</tr>";
        $query2 = 'SELECT * FROM boxes WHERE locatedOnNthColumn = "'.$column["columnId"].'" ORDER BY locatedOnNthRowFromBottom DESC';

        $results2 = $db->query($query2);

        while ($row = $results2->fetchArray()) {
            echo "<tr>";
            echo "<td>";

            echo "<u>Box Type</u>: ";
            echo $row["boxType"];

            echo "<br>";

            echo "<u>Box ID</u>: ";
            echo $row["boxId"];

            echo "<br>";
            echo "<br>";

            if ($row["boxType"] == "plant") {
                echo "<u>Plant Capacity</u>: ";
                echo $row["plantCapacity"];

                echo "<br>";

                echo "<u>Plant Type</u>:  ";
                echo $row["plantType"];

                echo "<br>";

                echo "<u>Plants are placed on</u>:  ";
                echo $row["plantsArePlacedOn"];

                echo "<br>";
                echo "<br>";
                echo "<u>Fogger ID</u>: ";
                echo $row["foggerId"];

                echo "<br>";

                echo "<u>Light ID</u>: ";
                echo $row["lightId"];

                echo "<br>";

                echo "<u>Light Sensor ID</u>: ";
                echo $row["lightSensorId"];

                echo "<br>";
            }

            echo "<u>Pump ID</u>: ";
            echo $row["pumpId"];

            echo "<br>";

            echo "<u>Water Level Sensor ID</u>: ";
            echo $row["waterLevelSensorId"];

            echo "<br>";
            echo "<br>";
            echo "<u>pH Min</u>: ";
            echo $row["phMin"];

            echo "<br>";

            echo "<u>pH Max</u>: ";
            echo $row["phMax"];

            echo "<br>";

            echo "<u>Current pH</u>: ";
            echo $row["phLevel"];

            echo "<br>";

            echo "<u>EC Min</u>: ";
            echo $row["ecMin"];

            echo "<br>";

            echo "<u>EC Max</u>: ";
            echo $row["ecMax"];

            echo "<br>";

            echo "<u>Current EC</u>: ";
            echo $row["ecLevel"];

            echo "<br>";

            echo "<u>Last pH and EC measurment time</u>: ";
            echo $row["phEcLastMeasuredOn"];

            echo "</td>";
            echo "</tr>";
        }

        echo "</table>";
        echo "</td>";
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
<meta http-equiv="refresh" content="5" >
<style>
h3 {
    font-family: arial, sans-serif;
    border-collapse: collapse;
    width: 95%;
}

h2, h4 {
    font-family: arial, sans-serif;
    font-size: 11pt;
    border-collapse: collapse;
    width: 95%;
}

table {
    font-family: arial, sans-serif;
    font-size: 11pt;
    border-collapse: collapse;
    width: 95%;
    vertical-align: top;
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
<h4 align="left"><i>

  Page refreshes every 5 second
  |   Current Unix time: <?php echo time(); ?>
  |   Server Time: <?php echo date('m/d/Y H:i:s', time()); ?>

  <br>

  Server IP Address: <?php echo $_SERVER['REMOTE_ADDR']; ?>
  |   Server Hostname: <?php echo gethostname();?>

</i></h4>

<h3 align="left">Devices Table</h3>
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
    <th onclick="sortTable(14)">Pump Mode</th>
  </tr>

  <?php createRowsForDevices(); ?>

</table>

<br>
<h3 align="left">Columns</h3>
<h4 align="left"><i>

  Boxes are order from top to bottom by the actual order

</i></h4>

<table>

<tr style="vertical-align: top;">
  <?php createRowsForColumns(); ?>
</tr>

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
