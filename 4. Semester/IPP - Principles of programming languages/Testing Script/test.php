<?php

const ARG_ERR = 10;
const FIN_ERR = 11;
const FOUT_ERR = 12;
const INTERNAL_ERR = 99;

const USAGE = "Usage:
php5.6 tests.php [--help] [--directory=dir] [--recursive] [--parse-script=file] [--int-script=file]\n";

$longopts  = array(
    "help",           // No value
	"directory:",
	"recursive",
	"parse-script:",
	"int-script:",
);

$args = getopt("", $longopts);

$dir = ".";
$parser = "parse.php";
$interpreter = "interpret.py";
$rec = false;

if(isset($args["help"]))
{
	if($argc != 2)
	{
		fprintf(STDERR, USAGE);
		exit(ARG_ERR);
	}
	else
	{
		fprintf(STDOUT, HELP);
		exit(0);
	}
}

if(isset($args["directory"]))
{
	$dir = $args["directory"];
}

if(isset($args["recursive"]))
{
	$rec = true;
}

if(isset($args["parse-script"]))
{
	$parser = $args["parse-script"];
}

if(isset($args["int-script"]))
{
	$interpreter = $args["int-script"];
}


if(!is_dir($dir))
{
	fprintf(STDERR, "{$dir} is not a directory!\n");
	exit(ARG_ERR);
}

$passed = 0;
$total = 0;
$test_results = array (
	);

runDirTests($dir, $rec);

$html_output = "<html>
<head>
	<title>Test results</title>
</head>
<body>
<h1 style=\"text-align: center;\">Test results</h1>
<hr />
<h3 style=\"text-align: center;\">Total tests passed<br />{$passed}/{$total}</h3>";

$out_end = "</body>
</html>";

foreach($test_results as $tr)
{
	$html_output = $html_output.$tr;
}
$html_output = $html_output.$out_end;

print($html_output);

exit(0);



function createFile($f)
{
	$ff = fopen($f, "a");
	if(!$ff) return false;
	fclose($ff);
	return true;
}

function prepTestFiles($t)
{
	$trc = $t.".rc";
	$tin = $t.".in";
	$tout = $t.".out";
	$rc = 0;
	if(!createFile($tin))
	{
		fprintf(STDERR, "Could not open file {$tin}!\n");
		exit(FIN_ERR);
	}	

	if(!createFile($tout))
	{
		fprintf(STDERR, "Could not open file {$tout}!\n");
		exit(FIN_ERR);
	}

	if(file_exists($trc))
	{
		$frc = fopen($trc, "r");
		if(!$frc)
		{
			fprintf(STDERR, "Could not open file {$trc}!\n");
			exit(FIN_ERR);
		}
		$count = fscanf($frc, "%d", $rc);
		if($count != 1)
		{
			fprintf(STDERR, "Incorrect format of file {$trc}!\n");
			exit(FIN_ERR);
		} 
		fclose($frc);
	}
	else
	{
		$frc = fopen($trc, "w");
		if(!$frc)
		{
			fprintf(STDERR, "Could not open file {$frcname}!\n");
			exit(FIN_ERR);
		}
		fprintf($frc, "0");
		fclose($frc);
	}
	return $rc;
}

function runTest($t)
{
	global $parser;
	global $interpreter;
	global $total;
	global $passed;
	global $test_results;

	$tsrc = $t.".src";
	$trc = $t.".rc";
	$tin = $t.".in";
	$tout = $t.".out";
	$ttmp = $t.".tmp";
	$tmyout = $t.".myout";
	$t_suc_p1 = "<p style=\"padding-left: 60px;\">";
	$t_suc_p2 = "<span style=\"color: #36e64b;\"> Success!</span></p>\n";
	$t_fail_p1 = "<p style=\"padding-left: 60px;\">";
	$t_fail_p2 = "<span style=\"color: #ff0000;\"> Fail!&nbsp;</span>";
	$t_fail_p3 = "</p>\n";
	$exprc = prepTestFiles($t);
	$total++;
	$descs = array(
	   				0 => array("pipe", "r"),  // stdin
	   				1 => array("pipe", "w"),  // stdout
	   				2 => array("pipe", "w")   // stderr
					);

	$process = proc_open("php5.6 \"".$parser."\" < \"".$tsrc."\" > \"".$ttmp."\"", $descs, $pipes);
	$parser_rc = 0;
	if (is_resource($process)) 
	{
		fclose($pipes[0]);

		$parse_out = stream_get_contents($pipes[1]);
		fclose($pipes[1]); 
		fclose($pipes[2]);

		$parser_rc = proc_close($process);
	}
	else
	{
		fprintf(STDERR, "Failed to run the parser!\n");
		exit(INTERNAL_ERR);
	}


	if($exprc == $parser_rc && $exprc != 0)
	{
		$tmp = $t_suc_p1."{$total}. {$t}".$t_suc_p2;
		$passed++;
		$test_results[$total] = $tmp;
		unlink($ttmp);
		return;
	}


	if($exprc != $parser_rc && $parser_rc != 0)
	{
		$tmp = $t_fail_p1."{$total}. {$t}".$t_fail_p2."Return code {$parser_rc}, expected {$exprc}.".$t_fail_p3;
		$test_results[$total] = $tmp;
		unlink($ttmp);
		return;
	}

	$descs = array(
	   				0 => array("pipe", "r"),  // stdin
	   				1 => array("pipe", "w"),  // stdout
	   				2 => array("pipe", "w")   // stderr
					);
	$process = proc_open("python3 \"".$interpreter."\" --source=\"".$ttmp."\" < \"".$tin."\" > \"".$tmyout."\"", $descs, $pipes);
	$int_out = NULL;
	$int_rc = 0;
	if (is_resource($process)) 
	{
		fclose($pipes[0]);

		$int_out = stream_get_contents($pipes[1]);
		fclose($pipes[1]);
		fclose($pipes[2]);

		$int_rc = proc_close($process);
	}
	else
	{
		fprintf(STDERR, "Failed to run the interpreter!\n");
		exit(INTERNAL_ERR);
	}
	
	unlink($ttmp);
	
	if($exprc == $int_rc && $int_rc != 0)
	{
		$tmp = $t_suc_p1."{$total}. {$t}".$t_suc_p2;
		$passed++;
		$test_results[$total] = $tmp;
		unlink($tmyout);
		return;
	}


	if($exprc != $int_rc)
	{
		$tmp = $t_fail_p1."{$total}. {$t}".$t_fail_p2."Return code {$int_rc}, expected {$exprc}.".$t_fail_p3;
		$test_results[$total] = $tmp;
		unlink($tmyout);
		return;
	}
	
	$dstring = "diff \"{$tout}\" \"{$tmyout}\"";
	exec($dstring, $output, $status);
	if($status != 0)
	{
		$tmp = $t_fail_p1."{$total}. {$t}".$t_fail_p2."Different output!".$t_fail_p3;
		$test_results[$total] = $tmp;
		unlink($tmyout);
		return;
	}
	else
	{
		$tmp = $t_suc_p1."{$total}. {$t}".$t_suc_p2;
		$passed++;
		$test_results[$total] = $tmp;
		unlink($tmyout);
		return;
	}	
	return;
}

function runDirTests($dir, $rec)
{
	$files = array_slice(scandir($dir),2);
	foreach($files as $f)
	{
		$f = $dir."/".$f;
		$ext = substr($f, -4);
		if(strcmp($ext, ".src") != 0)
			continue;
		$fname = substr($f, 0, strlen($f)-4);
		runTest($fname);		
		
	}
	
	if($rec)
	{
		foreach($files as $f)
		{
			$f = $dir."/".$f;
			if(is_dir($f))
			{
				runDirTests($f, $rec);
			}
		}		
	}
}

?>
