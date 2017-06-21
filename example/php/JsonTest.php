<?php

class JsonTest{
    private $db=null;
    function __construct(){
        $this->db=new mysqli('localhost', 'root', 'root', 'poptomdb');
        if($mysqli->connect_error) {
            die("could not connect database error: ". $this->db->error . "\n");
        }
        $this->db->query('set names utf8');
        // echo "connect database sucessfule!\n";
    }

    function getArrayData($sql=null){
        if($sql) {
            // echo "{$sql}\n";
            $result=$this->db->query($sql);
            if(!$result){
                die("sql error: " . $this->db->error);
            }

            $rows=array();
            while($row=$result->fetch_assoc()) {
                $rows[] = $row;
            }
            $result->free();
            return $rows;
        }
    }

    function getJsonData($arrayData){
        return json_encode($arrayData);
    }
    function __destruct(){
        $this->db->close();
    }
}

header('Content-type: text/json');
$jt=new JsonTest;
$arrayData=$jt->getArrayData("select * from p_custom");
$jsonData=$jt->getJsonData($arrayData);
echo $jsonData;
