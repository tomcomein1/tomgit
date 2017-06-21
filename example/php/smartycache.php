<?php
require('./setup.php');

$smarty=new SmartyPoptom();
$smarty->caching = true;

// only do db calls if cache doesn't exist
// 只有在缓存不存在时才调用数据库
if(!$smarty->is_cached("index.tpl"))
{
 // dummy up some data
 $address = "245 N 50th";
 $db_data = array(
     "City" => "Lincoln",
     "State" => "Nebraska",
    "Zip" => "68502"
     );
 $smarty->assign("Name","Fred");
 $smarty->assign("Address",$address);
 $smarty->assign($db_data);
}

// 捕获输出
$output = $smarty->fetch("index.tpl");


// do something with $output here
// 对将要输出的内容进行处理


echo $output;