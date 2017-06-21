<?php
define('SMARTY_DIR', 'libs/Smarty-3.1.13/libs/');
require_once(SMARTY_DIR.'Smarty.class.php');
$smarty=new Smarty();
$smarty->template_dir = 'smarty/templates/';
$smarty->compile_dir = 'smarty/templates_c/';
$smarty->config_dir = 'smarty/configs/';
$smarty->cache_dir = 'smarty/cache/';
$smarty->assign('name', 'Ned');
$smarty->display('index.tpl');
