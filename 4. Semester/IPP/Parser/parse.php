<?php

const ARG_ERR = 10;
const PARSE_ERR = 21;
const INTERNAL_ERR = 99;
const USAGE = "Usage:
php5.6 parse.php [--help]\n";
const HELP = "This script parses the input expected in the language, checks
its lexical and syntactic correctness and outputs the programs representation
in XML. This XML language is then parsed and processed by the subsequent module.";

const ERR_VARIABLE = "variable";
const ERR_SYMBOL = "symbol";
const ERR_CONSTANT = "constant";
const ERR_TYPE = "type";
const ERR_LABEL = "label";

$longopts  = array(
    "help",           // No value
);

$args = getopt("", $longopts);
$argCount = count($args);

if($argCount != $argc-1)
{
	fprintf(STDERR, USAGE);
	exit(ARG_ERR);
}

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

$in = fgets(STDIN);
$in = preg_replace("/(\s)+/", "", $in);
$in = preg_replace("/#.*/", "", $in);
$in = strtoupper($in);


if(strcmp($in, ".IPPCODE18") != 0)
{
	fprintf(STDERR, "IPPcode18 source files need to contain the \".IPPcode18\" header on the first line!\n");
	exit(PARSE_ERR);
}

$xw = xmlwriter_open_memory();
xmlwriter_set_indent($xw, 1);
$res = xmlwriter_set_indent_string($xw, ' ');

xmlwriter_start_document($xw, '1.0', 'UTF-8');

xmlwriter_start_element($xw, 'program');

xmlwriter_start_attribute($xw, 'language');
xmlwriter_text($xw, 'IPPcode18');
xmlwriter_end_attribute($xw);

$instrNum = 0;
$lineNum = 0;

while($in = fgets(STDIN))
{
	$in = preg_replace("/#.*/", "", $in);
	$in = preg_replace("/^(\s)+/", "", $in);
	$in = preg_replace("/(\s)+$/", "", $in);
	$in = preg_replace("/(\s)+/", " ", $in);
	if(strcmp("", $in) == 0) continue;
	$words = explode(" ", $in);
	$wordCount = count($words);
	$lineNum++;
	if($wordCount == 0)
		continue;

	xmlwriter_start_element($xw, 'instruction');
	$instrNum++;

	xmlwriter_start_attribute($xw, 'order');
	xmlwriter_text($xw, $instrNum);
	xmlwriter_end_attribute($xw);
	
	$instr = strtoupper($words[0]);

	xmlwriter_start_attribute($xw, 'opcode');
	xmlwriter_text($xw, $instr);
	xmlwriter_end_attribute($xw);

	switch($instr)
	{
		case "MOVE":
			if($wordCount != 3)
			{
				ErrorReport($instr, $lineNum, 0, NULL);
				exit(PARSE_ERR);
			}
			if(!ProcessVariable($words[1], 1, $xw))
			{
				ErrorReport($instr, $lineNum, 1, ERR_VARIABLE);
				exit(PARSE_ERR);
			}
			if(!ProcessSymbol($words[2], 2, $xw))
			{
				ErrorReport($instr, $lineNum, 2, ERR_SYMBOL);
				exit(PARSE_ERR);
			}
			break;
		case "CREATEFRAME":
			if($wordCount != 1)
			{
				ErrorReport($instr, $lineNum, 0, NULL);
				exit(PARSE_ERR);
			}
			break;
		case "PUSHFRAME":
			if($wordCount != 1)
			{
				ErrorReport($instr, $lineNum, 0, NULL);
				exit(PARSE_ERR);
			}
			break;
		case "POPFRAME":
			if($wordCount != 1)
			{
				ErrorReport($instr, $lineNum, 0, NULL);
				exit(PARSE_ERR);
			}
			break;
		case "DEFVAR":
			if($wordCount != 2)
			{
				ErrorReport($instr, $lineNum, 0, NULL);
				exit(PARSE_ERR);
			}
			if(!ProcessVariable($words[1], 1, $xw))
			{
				ErrorReport($instr, $lineNum, 1, ERR_VARIABLE);
				exit(PARSE_ERR);
			}
			break;
		case "CALL":
			if($wordCount != 2)
			{
				ErrorReport($instr, $lineNum, 0, NULL);
				exit(PARSE_ERR);
			}
			if(!ProcessLabel($words[1], 1, $xw))
			{
				ErrorReport($instr, $lineNum, 1, ERR_LABEL);
				exit(PARSE_ERR);
			}
			break;
		case "RETURN":
			if($wordCount != 1)
			{
				ErrorReport($instr, $lineNum, 0, NULL);
				exit(PARSE_ERR);
			}
			break;
		case "PUSHS":
			if($wordCount != 2)
			{
				ErrorReport($instr, $lineNum, 0, NULL);
				exit(PARSE_ERR);
			}
			if(!ProcessSymbol($words[1], 1, $xw))
			{
				ErrorReport($instr, $lineNum, 1, ERR_SYMBOL);
				exit(PARSE_ERR);
			}
			break;
		case "POPS":
			if($wordCount != 2)
			{
				ErrorReport($instr, $lineNum, 0, NULL);
				exit(PARSE_ERR);
			}
			if(!ProcessVariable($words[1], 1, $xw))
			{
				ErrorReport($instr, $lineNum, 1, ERR_VARIABLE);
				exit(PARSE_ERR);
			}
			break;
		case "ADD":
			if($wordCount != 4)
			{
				ErrorReport($instr, $lineNum, 0, NULL);
				exit(PARSE_ERR);
			}
			if(!ProcessVariable($words[1], 1, $xw))
			{
				ErrorReport($instr, $lineNum, 1, ERR_VARIABLE);
				exit(PARSE_ERR);
			}
			if(!ProcessSymbol($words[2], 2, $xw))
			{
				ErrorReport($instr, $lineNum, 2, ERR_SYMBOL);
				exit(PARSE_ERR);
			}
			if(!ProcessSymbol($words[3], 3, $xw))
			{
				ErrorReport($instr, $lineNum, 3, ERR_SYMBOL);
				exit(PARSE_ERR);
			}
			break;
		case "SUB":
			if($wordCount != 4)
			{
				ErrorReport($instr, $lineNum, 0, NULL);
				exit(PARSE_ERR);
			}
			if(!ProcessVariable($words[1], 1, $xw))
			{
				ErrorReport($instr, $lineNum, 1, ERR_VARIABLE);
				exit(PARSE_ERR);
			}
			if(!ProcessSymbol($words[2], 2, $xw))
			{
				ErrorReport($instr, $lineNum, 2, ERR_SYMBOL);
				exit(PARSE_ERR);
			}
			if(!ProcessSymbol($words[3], 3, $xw))
			{
				ErrorReport($instr, $lineNum, 3, ERR_SYMBOL);
				exit(PARSE_ERR);
			}
			break;
		case "MUL":
			if($wordCount != 4)
			{
				ErrorReport($instr, $lineNum, 0, NULL);
				exit(PARSE_ERR);
			}
			if(!ProcessVariable($words[1], 1, $xw))
			{
				ErrorReport($instr, $lineNum, 1, ERR_VARIABLE);
				exit(PARSE_ERR);
			}
			if(!ProcessSymbol($words[2], 2, $xw))
			{
				ErrorReport($instr, $lineNum, 2, ERR_SYMBOL);
				exit(PARSE_ERR);
			}
			if(!ProcessSymbol($words[3], 3, $xw))
			{
				ErrorReport($instr, $lineNum, 3, ERR_SYMBOL);
				exit(PARSE_ERR);
			}
			break;
		case "IDIV":
			if($wordCount != 4)
			{
				ErrorReport($instr, $lineNum, 0, NULL);
				exit(PARSE_ERR);
			}
			if(!ProcessVariable($words[1], 1, $xw))
			{
				ErrorReport($instr, $lineNum, 1, ERR_VARIABLE);
				exit(PARSE_ERR);
			}
			if(!ProcessSymbol($words[2], 2, $xw))
			{
				ErrorReport($instr, $lineNum, 2, ERR_SYMBOL);
				exit(PARSE_ERR);
			}
			if(!ProcessSymbol($words[3], 3, $xw))
			{
				ErrorReport($instr, $lineNum, 3, ERR_SYMBOL);
				exit(PARSE_ERR);
			}
			break;
		case "LT":
			if($wordCount != 4)
			{
				ErrorReport($instr, $lineNum, 0, NULL);
				exit(PARSE_ERR);
			}
			if(!ProcessVariable($words[1], 1, $xw))
			{
				ErrorReport($instr, $lineNum, 1, ERR_VARIABLE);
				exit(PARSE_ERR);
			}
			if(!ProcessSymbol($words[2], 2, $xw))
			{
				ErrorReport($instr, $lineNum, 2, ERR_SYMBOL);
				exit(PARSE_ERR);
			}
			if(!ProcessSymbol($words[3], 3, $xw))
			{
				ErrorReport($instr, $lineNum, 3, ERR_SYMBOL);
				exit(PARSE_ERR);
			}
			break;
		case "GT":
			if($wordCount != 4)
			{
				ErrorReport($instr, $lineNum, 0, NULL);
				exit(PARSE_ERR);
			}
			if(!ProcessVariable($words[1], 1, $xw))
			{
				ErrorReport($instr, $lineNum, 1, ERR_VARIABLE);
				exit(PARSE_ERR);
			}
			if(!ProcessSymbol($words[2], 2, $xw))
			{
				ErrorReport($instr, $lineNum, 2, ERR_SYMBOL);
				exit(PARSE_ERR);
			}
			if(!ProcessSymbol($words[3], 3, $xw))
			{
				ErrorReport($instr, $lineNum, 3, ERR_SYMBOL);
				exit(PARSE_ERR);
			}
			break;
		case "EQ":
			if($wordCount != 4)
			{
				ErrorReport($instr, $lineNum, 0, NULL);
				exit(PARSE_ERR);
			}
			if(!ProcessVariable($words[1], 1, $xw))
			{
				ErrorReport($instr, $lineNum, 1, ERR_VARIABLE);
				exit(PARSE_ERR);
			}
			if(!ProcessSymbol($words[2], 2, $xw))
			{
				ErrorReport($instr, $lineNum, 2, ERR_SYMBOL);
				exit(PARSE_ERR);
			}
			if(!ProcessSymbol($words[3], 3, $xw))
			{
				ErrorReport($instr, $lineNum, 3, ERR_SYMBOL);
				exit(PARSE_ERR);
			}
			break;
		case "AND":
			if($wordCount != 4)
			{
				ErrorReport($instr, $lineNum, 0, NULL);
				exit(PARSE_ERR);
			}
			if(!ProcessVariable($words[1], 1, $xw))
			{
				ErrorReport($instr, $lineNum, 1, ERR_VARIABLE);
				exit(PARSE_ERR);
			}
			if(!ProcessSymbol($words[2], 2, $xw))
			{
				ErrorReport($instr, $lineNum, 2, ERR_SYMBOL);
				exit(PARSE_ERR);
			}
			if(!ProcessSymbol($words[3], 3, $xw))
			{
				ErrorReport($instr, $lineNum, 3, ERR_SYMBOL);
				exit(PARSE_ERR);
			}
			break;
		case "OR":
			if($wordCount != 4)
			{
				ErrorReport($instr, $lineNum, 0, NULL);
				exit(PARSE_ERR);
			}
			if(!ProcessVariable($words[1], 1, $xw))
			{
				ErrorReport($instr, $lineNum, 1, ERR_VARIABLE);
				exit(PARSE_ERR);
			}
			if(!ProcessSymbol($words[2], 2, $xw))
			{
				ErrorReport($instr, $lineNum, 2, ERR_SYMBOL);
				exit(PARSE_ERR);
			}
			if(!ProcessSymbol($words[3], 3, $xw))
			{
				ErrorReport($instr, $lineNum, 3, ERR_SYMBOL);
				exit(PARSE_ERR);
			}
			break;
		case "NOT":
			if($wordCount != 3)
			{
				ErrorReport($instr, $lineNum, 0, NULL);
				exit(PARSE_ERR);
			}
			if(!ProcessVariable($words[1], 1, $xw))
			{
				ErrorReport($instr, $lineNum, 1, ERR_VARIABLE);
				exit(PARSE_ERR);
			}
			if(!ProcessSymbol($words[2], 2, $xw))
			{
				ErrorReport($instr, $lineNum, 2, ERR_SYMBOL);
				exit(PARSE_ERR);
			}
			break;
		case "INT2CHAR":
			if($wordCount != 3)
			{
				ErrorReport($instr, $lineNum, 0, NULL);
				exit(PARSE_ERR);
			}
			if(!ProcessVariable($words[1], 1, $xw))
			{
				ErrorReport($instr, $lineNum, 1, ERR_VARIABLE);
				exit(PARSE_ERR);
			}
			if(!ProcessSymbol($words[2], 2, $xw))
			{
				ErrorReport($instr, $lineNum, 2, ERR_SYMBOL);
				exit(PARSE_ERR);
			}
			break;
		case "STRI2INT":
			if($wordCount != 4)
			{
				ErrorReport($instr, $lineNum, 0, NULL);
				exit(PARSE_ERR);
			}
			if(!ProcessVariable($words[1], 1, $xw))
			{
				ErrorReport($instr, $lineNum, 1, ERR_VARIABLE);
				exit(PARSE_ERR);
			}
			if(!ProcessSymbol($words[2], 2, $xw))
			{
				ErrorReport($instr, $lineNum, 2, ERR_SYMBOL);
				exit(PARSE_ERR);
			}
			if(!ProcessSymbol($words[3], 3, $xw))
			{
				ErrorReport($instr, $lineNum, 3, ERR_SYMBOL);
				exit(PARSE_ERR);
			}
			break;
		case "READ":
			if($wordCount != 3)
			{
				ErrorReport($instr, $lineNum, 0, NULL);
				exit(PARSE_ERR);
			}
			if(!ProcessVariable($words[1], 1, $xw))
			{
				ErrorReport($instr, $lineNum, 1, ERR_VARIABLE);
				exit(PARSE_ERR);
			}
			if(!ProcessType($words[2], 2, $xw))
			{
				ErrorReport($instr, $lineNum, 2, ERR_TYPE);
				exit(PARSE_ERR);
			}
			break;
		case "WRITE":
			if($wordCount != 2)
			{
				ErrorReport($instr, $lineNum, 0, NULL);
				exit(PARSE_ERR);
			}
			if(!ProcessSymbol($words[1], 1, $xw))
			{
				ErrorReport($instr, $lineNum, 1, ERR_SYMBOL);
				exit(PARSE_ERR);
			}
			break;
		case "CONCAT":
			if($wordCount != 4)
			{
				ErrorReport($instr, $lineNum, 0, NULL);
				exit(PARSE_ERR);
			}
			if(!ProcessVariable($words[1], 1, $xw))
			{
				ErrorReport($instr, $lineNum, 1, ERR_VARIABLE);
				exit(PARSE_ERR);
			}
			if(!ProcessSymbol($words[2], 2, $xw))
			{
				ErrorReport($instr, $lineNum, 2, ERR_SYMBOL);
				exit(PARSE_ERR);
			}
			if(!ProcessSymbol($words[3], 3, $xw))
			{
				ErrorReport($instr, $lineNum, 3, ERR_SYMBOL);
				exit(PARSE_ERR);
			}
			break;
		case "STRLEN":
			if($wordCount != 3)
			{
				ErrorReport($instr, $lineNum, 0, NULL);
				exit(PARSE_ERR);
			}
			if(!ProcessVariable($words[1], 1, $xw))
			{
				ErrorReport($instr, $lineNum, 1, ERR_VARIABLE);
				exit(PARSE_ERR);
			}
			if(!ProcessSymbol($words[2], 2, $xw))
			{
				ErrorReport($instr, $lineNum, 2, ERR_SYMBOL);
				exit(PARSE_ERR);
			}
			break;
		case "GETCHAR":
			if($wordCount != 4)
			{
				ErrorReport($instr, $lineNum, 0, NULL);
				exit(PARSE_ERR);
			}
			if(!ProcessVariable($words[1], 1, $xw))
			{
				ErrorReport($instr, $lineNum, 1, ERR_VARIABLE);
				exit(PARSE_ERR);
			}
			if(!ProcessSymbol($words[2], 2, $xw))
			{
				ErrorReport($instr, $lineNum, 2, ERR_SYMBOL);
				exit(PARSE_ERR);
			}
			if(!ProcessSymbol($words[3], 3, $xw))
			{
				ErrorReport($instr, $lineNum, 3, ERR_SYMBOL);
				exit(PARSE_ERR);
			}
			break;
		case "SETCHAR":
			if($wordCount != 4)
			{
				ErrorReport($instr, $lineNum, 0, NULL);
				exit(PARSE_ERR);
			}
			if(!ProcessVariable($words[1], 1, $xw))
			{
				ErrorReport($instr, $lineNum, 1, ERR_VARIABLE);
				exit(PARSE_ERR);
			}
			if(!ProcessSymbol($words[2], 2, $xw))
			{
				ErrorReport($instr, $lineNum, 2, ERR_SYMBOL);
				exit(PARSE_ERR);
			}
			if(!ProcessSymbol($words[3], 3, $xw))
			{
				ErrorReport($instr, $lineNum, 3, ERR_SYMBOL);
				exit(PARSE_ERR);
			}
			break;
		case "TYPE":
			if($wordCount != 3)
			{
				ErrorReport($instr, $lineNum, 0, NULL);
				exit(PARSE_ERR);
			}
			if(!ProcessVariable($words[1], 1, $xw))
			{
				ErrorReport($instr, $lineNum, 1, ERR_VARIABLE);
				exit(PARSE_ERR);
			}
			if(!ProcessSymbol($words[2], 2, $xw))
			{
				ErrorReport($instr, $lineNum, 2, ERR_SYMBOL);
				exit(PARSE_ERR);
			}
			break;
		case "LABEL":
			if($wordCount != 2)
			{
				ErrorReport($instr, $lineNum, 0, NULL);
				exit(PARSE_ERR);
			}
			if(!ProcessLabel($words[1], 1, $xw))
			{
				ErrorReport($instr, $lineNum, 1, ERR_LABEL);
				exit(PARSE_ERR);
			}
			break;
		case "JUMP":
			if($wordCount != 2)
			{
				ErrorReport($instr, $lineNum, 0, NULL);
				exit(PARSE_ERR);
			}
			if(!ProcessLabel($words[1], 1, $xw))
			{
				ErrorReport($instr, $lineNum, 1, ERR_LABEL);
				exit(PARSE_ERR);
			}
			break;
		case "JUMPIFEQ":
			if($wordCount != 4)
			{
				ErrorReport($instr, $lineNum, 0, NULL);
				exit(PARSE_ERR);
			}
			if(!ProcessLabel($words[1], 1, $xw))
			{
				ErrorReport($instr, $lineNum, 1, ERR_LABEL);
				exit(PARSE_ERR);
			}
			if(!ProcessSymbol($words[2], 2, $xw))
			{
				ErrorReport($instr, $lineNum, 2, ERR_SYMBOL);
				exit(PARSE_ERR);
			}
			if(!ProcessSymbol($words[3], 3, $xw))
			{
				ErrorReport($instr, $lineNum, 3, ERR_SYMBOL);
				exit(PARSE_ERR);
			}
			break;
		case "JUMPIFNEQ":
			if($wordCount != 4)
			{
				ErrorReport($instr, $lineNum, 0, NULL);
				exit(PARSE_ERR);
			}
			if(!ProcessLabel($words[1], 1, $xw))
			{
				ErrorReport($instr, $lineNum, 1, ERR_LABEL);
				exit(PARSE_ERR);
			}
			if(!ProcessSymbol($words[2], 2, $xw))
			{
				ErrorReport($instr, $lineNum, 2, ERR_SYMBOL);
				exit(PARSE_ERR);
			}
			if(!ProcessSymbol($words[3], 3, $xw))
			{
				ErrorReport($instr, $lineNum, 3, ERR_SYMBOL);
				exit(PARSE_ERR);
			}
			break;
		case "DPRINT":
			if($wordCount != 2)
			{
				ErrorReport($instr, $lineNum, 0, NULL);
				exit(PARSE_ERR);
			}
			if(!ProcessSymbol($words[1], 1, $xw))
			{
				ErrorReport($instr, $lineNum, 1, ERR_SYMBOL);
				exit(PARSE_ERR);
			}
			break;
		case "BREAK":
			if($wordCount != 1)
			{
				ErrorReport($instr, $lineNum, 0, NULL);
				exit(PARSE_ERR);
			}
			break;
		default:
			ErrorReport($instr, $lineNum, -1, NULL);
			exit(PARSE_ERR);
			break;
	}

	xmlwriter_end_element($xw);
}
	
xmlwriter_end_element($xw);

xmlwriter_end_document($xw);

echo xmlwriter_output_memory($xw);

function ProcessSymbol($symbol, $order, &$xw)
{
	if(ProcessConstant($symbol, $order, $xw))
		return true;
	else if(ProcessVariable($symbol, $order, $xw))
		return true;
	return false;
}

function ProcessVariable($var, $order, &$xw)
{
	if(CheckVariable($var))
	{
		xmlwriter_start_element($xw, "arg{$order}");

		xmlwriter_start_attribute($xw, 'type');
		xmlwriter_text($xw, 'var');
		xmlwriter_end_attribute($xw);

		xmlwriter_text($xw, $var);

		xmlwriter_end_element($xw);

		return true;
	}

	return false;
}

function ProcessConstant($con, $order, &$xw)
{
	if(CheckConstant($con, $type))
	{
		xmlwriter_start_element($xw, "arg{$order}");

		xmlwriter_start_attribute($xw, 'type');
		xmlwriter_text($xw, $type);
		xmlwriter_end_attribute($xw);

		xmlwriter_text($xw, $con);

		xmlwriter_end_element($xw);

		return true;
	}

	return false;
}

function ProcessType($type, $order, &$xw)
{
	if(CheckType($type))
	{
		xmlwriter_start_element($xw, "arg{$order}");

		xmlwriter_start_attribute($xw, 'type');
		xmlwriter_text($xw, 'type');
		xmlwriter_end_attribute($xw);

		xmlwriter_text($xw, $type);

		xmlwriter_end_element($xw);

		return true;
	}

	return false;
}

function ProcessLabel($label, $order, &$xw)
{
	if(CheckID($label))
	{
		xmlwriter_start_element($xw, "arg{$order}");

		xmlwriter_start_attribute($xw, 'type');
		xmlwriter_text($xw, 'label');
		xmlwriter_end_attribute($xw);

		xmlwriter_text($xw, $label);

		xmlwriter_end_element($xw);

		return true;
	}

	return false;
}

function CheckSymbol($symbol)
{
	return CheckVariable($symbol) || CheckConstant($symbol);
}

function CheckVariable($var)
{
	if(preg_match("/^([TLG]F@.+)$/", $var))
	{
		return CheckID(substr($var, 3, strlen($var)));
	}
}

function CheckConstant(&$con, &$type)
{
	if(preg_match("/^(string@(\\\\[0-9]{3}|[^[:cntrl:]\s#\\\\])*)$/", $con))
	{
		$con = substr($con, 7, strlen($con));
		$con = htmlspecialchars($con, ENT_XML1, 'UTF-8');
		$type = 'string';
		return true;
	}
	else if(preg_match("/^bool@(true|false)$/", $con))
	{
		$con = substr($con, 5, strlen($con));
		$type = 'bool';
		return true;
	}
	else if(preg_match("/^int@.+$/", $con))
	{
		$con = substr($con, 4, strlen($con));
		$type = 'int';
		return true;
	}
	else
		return false;
}

function CheckType($type)
{
	return preg_match("/^(int|bool|string)$/", $type);
}

function CheckID($id)
{
	return preg_match("/^([a-zA-Z_\\-\\$&%\\*][0-9a-zA-Z_\\-\\$&%\\*]*)$/", $id);
}

function ErrorReport($instruction, $lineNum, $order, $expected)
{
	if($order == -1)
	{
		fprintf(STDERR, "Unknown instruction {$instruction} on the line {$lineNum}!\n");
	}
	else if($order == 0)
	{
		fprintf(STDERR, "Instruction {$instruction} on the line {$lineNum} has incorrect number of operands!\n");
	}
	else
	{
		if($order == 1)
			$orderWord = "first";
		else if($order == 2)
			$orderWord = "second";
		else 
			$orderWord = "third";
						
		fprintf(STDERR, "Instruction {$instruction} on the line {$lineNum} expects a {$expected} as its {$orderWord} operand!\n");
	}
}

?>
