<html>
<title> Helsinki Weather </title>

<?php
    // Set automatic refresh
    echo("<meta http-equiv='refresh' content='30'>");

    // Set measurement interval to file interval.txt
    $path = $_SERVER['DOCUMENT_ROOT'] . '/interval.txt';
    if (isset($_POST['1']))
    {
        file_put_contents($path, "1");
    }
    if (isset($_POST['10']))
    {
        file_put_contents($path, "10");
    }
    if (isset($_POST['60']))
    {
        file_put_contents($path, "60");
    }
    $setting = file_get_contents($path);

    // Get measurement data from file raw.html
    $path = $_SERVER['DOCUMENT_ROOT'] . '/raw.html';
    $data = file_get_contents($path);
    $num_last = 16;
    $pos1 = strpos($data, "<p>", 0);
    $pos2 = strpos($data, "<p>", $pos1 + 1);
    $length = $pos2 - $pos1;
    $first = strpos($data, "<p>", strlen($data) - $length - 1);	
    $act_num = 1;

    // Get position of first data for graphics
    for ($i = 0; $i < $num_last - 1; $i++)
    {	
        if ($first < $length)
        {
            break;
        }
	
        $first = strpos($data, "<p>", $first - $length - 1);
        $act_num++;
    }
    $temp_str = "Temperature:";
    $hum_str = "Humidity:";
    $temp_length = strlen($temp_str);
    $hum_length = strlen($hum_str);	

    // Get data for graphics
    for ($i = 0; $i < $act_num; $i++)
    {
        $pos = $first;
        $time[$i] = substr($data, $pos + 14, 5);
        $pos = strpos($data, $temp_str, $pos);
        $pos2 = strpos($data, " ", $pos + $temp_length + 1);
        $temperature[$i] = substr($data, $pos + $temp_length + 1, $pos2 - $pos - $temp_length - 1);
        $pos = strpos($data, $hum_str, $pos);
        $pos2 = strpos($data, "<", $pos);
        $humidity[$i] = substr($data, $pos + $hum_length + 1, $pos2 - $pos - $hum_length - 1);
        $first = strpos($data, "<p>", $first + $length - 1);
    }

    $temp_min = min($temperature);
    $temp_max = max($temperature);
    $temp_axis_min = 5 * round($temp_min / 5 - 0.25) - 5;
    $temp_axis_max = 5 * round($temp_max / 5 + 0.25) + 5;
    $temperatures = array_combine($time, $temperature);
    $humidities = array_combine($time, $humidity);
?>

    <body>
    <header>
         <h3><font face="verdana" color="blue">Weather in Helsinki at <?php printf('%s', $time[$act_num - 1]); ?></font></h3>
             <p><font face="verdana">Temperature: <?php printf('<b>%s &deg;C</b>', $temperature[$act_num - 1]); ?></font></p>
            <p><font face="verdana">Humidity: <?php printf('<b>%s &percnt;</b>', $humidity[$act_num - 1]); ?></font></p>
    </header>
    <header>
        <h4><font face="verdana" color="blue">Temperature [&deg;C]</font></h4>
    </header>

<?php
    // Create graphics for temperature history
    require_once($_SERVER['DOCUMENT_ROOT'] . '/SVGGraph/SVGGraph.php');
    $settings = array(
        'axis_min_v'       => $temp_axis_min,
        'axis_max_v'       => $temp_axis_max,
	    'axis_font_size'   => 12,
    );
    $graph = new SVGGraph(700, 300, $settings);
    $graph->Values($temperatures);
    echo $graph->Fetch('LineGraph', false);
?>

    <header>
        <h4><font face="verdana" color="blue">Humidity [%]</font></h4>
    </header>

<?php
    // Create graphics for humidity history
    $settings = array(
        'axis_min_v'       => 0,
        'axis_max_v'       => 100,
	    'axis_font_size'   => 12,
    );
    $graph = new SVGGraph(700, 300, $settings);
    $graph->Values($humidities);
    echo $graph->Fetch('LineGraph', false);
?>

    <header>
        <h4><font face="verdana" color="blue">Measurement interval: <?php echo $setting?> min</font></h4>
    </header>
        <form action="weather.php" method="post">
            <input style="width: 5%; margin-right: 32px; font-family: sans-serif; font-size: 14px; font-weight: bold;" input type="submit" name="1" value="1" <?php if ($setting == '1'){ ?> disabled <?php } ?>>
            <input style="width: 5%; margin-right: 32px; font-family: sans-serif; font-size: 14px; font-weight: bold;" input type="submit" name="10" value="10" <?php if ($setting == '10'){ ?> disabled <?php } ?>>
            <input style="width: 5%; font-family: sans-serif; font-size: 14px; font-weight: bold;" input type="submit" name="60" value="60" <?php if ($setting == '60'){ ?> disabled <?php } ?>>
        </form>

<?php
    echo $graph->FetchJavascript();
?>

    </body>
</html>
