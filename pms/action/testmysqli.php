<?php
//test db_mysqli
include_once "libs/classes/db_mysqli.class.php";
include_once "db.pub.php";

$db=open_database();
public_router();
//public_insert($db, 'p_custom');
$db->close();

// $structs=array();
// $structs=$db->desc_table("contact");
// // print_r($structs);
// foreach($structs as $k=>$v) {
//     foreach($v as $r=>$value){
//         echo $r."=>".$value."\n";
//     }
// }

// $cols=array();
// $cols=$db->get_table_cols("contact");
// foreach($cols as $k=>$v){
//     echo $k."=>".$v."\n";
// }


// //$colums=array("name"=>"1234", "email"=>"t33@qq.com", "message"=>"xxx");
// //$ch=array("tbl_name"=>"contact", "column_key"=>"id");
// $ch=array("tbl_name"=>"contact", "colum_key"=>"");
// $colums=array("name", "email");
// $sql=new MakeSql("select", $colums, $ch);

// echo $sql;

