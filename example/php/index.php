<?php
require('./setup.php');

$smarty=new SmartyPoptom();
$smarty->assign('name', '多儿');
$smarty->assign('title', '泡泡糖儿童摄影');
// $smarty->assign("tds", array("Fist", "Last", "Address", "Hello", "sfds"));
$smarty->display('index.tpl');