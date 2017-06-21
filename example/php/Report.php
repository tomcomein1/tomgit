<?php
class Report{
  private $inputs=array();

  function __construct() { }

  function setInputs() {
     $varArray=func_get_args();
     foreach($varArray as $name) {
       $this->inputs[$name]=$_REQUEST[$name];
     }
  }

  function getInputs() {
     return $this->inputs;
  }

  function setElement($key, $value) {
     $this->inputs[$key]=$value;
  }

  function getElement($key) {
     return $this->inputs[$key];
  }

  function showInputs() {
     foreach($this->inputs as $name => $value) {
       echo $name.'=>'.$value;
     }
  }
}

class Views{
}

$rep=new Report();
$rep->setInputs('name');
$rep->showInputs();

