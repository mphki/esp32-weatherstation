<?php
// Collect temperature and humidity to file raw.html with timestamp
date_default_timezone_set("Europe/Helsinki");
$TimeStamp = date("Y-m-d H:i:s");
$path = $_SERVER['DOCUMENT_ROOT'] . '/raw.html';
$var1 = $_REQUEST['Temperature'];
$var2 = $_REQUEST['Humidity'];
$str = "<p>" . $TimeStamp . "&nbsp;&nbsp;&nbsp;&nbsp;Temperature: " . $var1 . " &deg;C" . "&nbsp;&nbsp;&nbsp;&nbsp;Humidity: " . $var2 . "</p>";
file_put_contents($path, $str, FILE_APPEND);
?>