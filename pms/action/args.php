<?php

function table_head(){
	//echo func_num_args();
	$str="<tr>";
	$varArray=func_get_args();
	foreach($varArray as $value) {
	  $str .=  "<th>" .$value . "</th>\n";
	}
	$str.="</tr>\n";
	return $str;
}

$th=table_head("fdsa","dfa","fdadf");
echo $th;
