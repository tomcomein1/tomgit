<?php
class Hook {

    public $HOOK_PATH;
    public $PATH; //完整钩子文件目录
    public $object;

    //调用的时候这个类使用的时候，必须在系统的执行流程当中
    public function __construct() {

        $this->HOOK_PATH = ''; //项目的路径,根据你的项目计算路径
        $current_path = str_replace("\\", "/", getcwd()); //获取当前目录
        //这个地方在实际用的时候看你需要调整
        $this->PATH = $current_path . $this->HOOK_PATH;
    }

    /* 注册钩子 也可以叫做运行钩子
     * $class       类名称
     * $function    方法
     * $param       方法参数
     */

    public function run_hook($class, $function, $param = array()) {
        include_once $this->PATH . '/' . $class . '.class.php';
//        var_dump($this->PATH . '/' . $class . '.class.php');
//        call_user_func_array(array($class, $function), $param);//只能调用类的静态方法
//        call_user_func(array($class, $function), $param);  //只能调用类的静态方法
//        其他写法
        $this->object = new $class();
        $this->object->$function($param); //这样就可以不用调用静态方法
    }

    //返回当前已经所有的钩子类和方法  不要当前方法调用这个核心类,需要稍微改造，在$hook_array[$key]['function']的返回方法名的时候
    public function get_all_class() {
        //搜寻hook目录下的所有钩子文件，返回数组
//        $this->PATH
//        var_dump($this->PATH);
        $file_array = scandir($this->PATH);
        $hook_array = array();
        foreach ($file_array as $key => $value) {
            if (strpos($value, '.class.php') == true) {     //扫描路径绝对不能和这个类本身在一个同一个目录下，不然会出现重复声明的同名类
                $name = explode('.', $value);
                $hook_array[$key]['name'] = $name['0'] . '钩子类';
                $hook_array[$key]['url'] = $this->PATH . '/' . $value;
//                include $hook_array[$key]['url'];
//                $cc = new $name['0']();
//                $hook_array[$key]['function'][] = get_class_methods($cc);
//                $hook_array[$key]['function']['param'][] = get_class_vars($class_name); //获取方法变量

            }
        }
        return $hook_array;
    }
}

$rr = new hook();
//$ee = $rr->get_all_class();
$rr->run_hook('Testhook','showStr',array());
//echo '<pre>';
//print_r($ee);
//echo '</pre>';
