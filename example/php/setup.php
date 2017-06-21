<?php
//load Smarty library
define('SMARTY_DIR', 'libs/Smarty-3.1.13/libs/');
require_once(SMARTY_DIR.'Smarty.class.php');

class SmartyPoptom extends Smarty{
    function __construct(){
        parent::__construct();
        $this->template_dir = 'smarty/templates/';
        $this->compile_dir = 'smarty/templates_c/';
        $this->config_dir = 'smarty/configs/';
        $this->cache_dir = 'smarty/cache/';

        $this->caching=false;
    }
}
