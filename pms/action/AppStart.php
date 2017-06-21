<?php

require_once "libs/classes/db_mysqli.class.php";

class SingleConfig {
  private $props=array();
  private $db=null;
  private static $instance=null;

  private function __construct(){
   if(empty($props)) {
     $cfg=array( "debug"=>false,
       "autoconnect"=>1,
       "hostname"=>"localhost",
       "username"=>"root",
       "password"=>"root",
       "database"=>"poptomdb"
       );
     $this->setProps($cfg);
   }
 }
 public static function getInstance(){
  if(!self::$instance) {
    self::$instance = new self();
  }
  return self::$instance;
  }
  public function setProps($props){
    $this->props = $props;
  }
  public function getProps(){
    return $this->props;
  }
  public function setPropsKey($key, $val){
    $this->props[$key]=$val;
  }
  public function getPropsKey($key){
    return $this->props[$key];
  }
}

/**数据的处理抽象类 **/
abstract class AbsDataProcess{
  //保存处理数据
  protected $qdata=array();
  //保存处理返回值
  protected $result=false;

  abstract public function ohter_process();
  abstract public function insert($db, $table);
  abstract public function update($db, $table);
  abstract public function delete($db, $table);


  public function getResult(){
    return $this->result;
  }
}

class AdapterProcess extends AbsDataProcess{
  public function __construct(){}
  public function ohter_process() {
  }
  public function update($db, $table){
    $swhere=$this->getWhere($db, $table);
    $data=$this->getData($db, $table);
    $this->result=$db->update($data, $table, $swhere);
    return $this->result;
  }
  public function insert($db, $table) {
    $data=$this->getData($db, $table);
    $this->result=$db->insert($data, $table);
    return $this->result;
  }

  public function getWhere($db, $table){
    $key=$db->get_primary($table);
    $where=$key .'='. $this->getRequestValue($key);
    return $where;
  }

  public function getData($db, $table){
    echo " {$table}\n";
    //取字段
    // print_r($db);
    $fields=$db->get_fields($table);
    // print_r($fields);
     //取健值
    $key=$db->get_primary($table);
    // print_r($key);
    $data=array();
    foreach($fields as $colname=>$coltype){
     if($key === $colname) {
      continue;
      }
      if($colname==="datetime") {
         $data[datetime]=date("Y-m-d h:i:sa");
      } else if($colname==="reg_date") {
         $data[reg_date]=date("Y-m-d");
      } else {
       $data[$colname] = $colname;
       $data[$colname] = $this->getRequestValue($colname);
      }
    }

     print_r($data);
     return $data;
  }

  public function getRequestValue($colname){
    // return $_Request[$colname];
    return $colname;
  }
  public function delete($db, $table){
    print_r($table."\n");
    if($table==="p_acctreg")
    {
      $regid=$this->getRequestValue("regid");
      $datetime=date("Y-m-d h:i:sa");
      $sql="update p_acctreg set is_valid='N',datetime='$datetime' where regid=$regid ";
      return $db->query($sql);
    }else{
      $where=$this->getWhere($db, $table);
    }

    echo $where."\n";
    $this->result=$db->delete($table, $where);
    return $this->result;
  }
}

class AppStart {
   // private $cfg;
   private $db;
   protected $param=array();
   private $table;
   private $method;

   public function __construct( ) {
      $this->cfg=SingleConfig::getInstance();
      $this->db = new db_mysqli();
      $this->db->open($this->cfg->getProps() );
      echo "connect database...\n";
   }

   public function setTable($table){
    $this->table=$table;
  }
  public function setMethod($method){
    $this->method=$method;
  }

  public function  setParams($param){
    $this->params[]=$param;
  }
  public function  getDbHandle(){
    return $this->db;
  }
  public function exec_flow(){

  //    var_dump($this->db);
    echo "$this->method\n";
    $proc=new AdapterProcess();
    switch($this->method){
      case "insert": return $proc->insert($this->db, $this->table);
      case "update": return $proc->update($this->db, $this->table);
      case "delete": return $proc->delete($this->db, $this->table);
      default: {
        $this->setParams($this->db);
        return $this->callMethod();
      }
    }
  }

  public function callMethod( ){
    //call_user_func_array(array('MyFunction', $this->method), $this->params);
    if(function_exists ($this->method)) {
      return call_user_func_array($this->method, $this->params);
      // return call_user_func_array($this->method, $this->params);
    } else {
      echo "not define this function ".$this->method;
    }
  }

  public function __destruct() {
     if($this->db) {
       $this->db->close();
       echo "free database...\n";
     }
  }
}

// class MyFunction{
//   public static function test_call($db){
//       echo "fuck you!\n";
//   }
// }
function test_call($db){
      echo "fuck you!\n";
}

$appstart=new AppStart();
$appstart->setMethod("test_call");
$appstart->setTable("p_user");
$appstart->exec_flow();
