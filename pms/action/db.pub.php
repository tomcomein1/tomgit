<?php
include_once "dbconfig.php";
include_once "libs/classes/db_mysqli.class.php";

function open_database(){
    global $host, $user, $pass, $dbname;
    $config=array(
        "autoconnect"=>1,
        "hostname"=>$host,
        "username"=>$user,
        "password"=>$pass,
        "database"=>$dbname
        );
    $db=new db_mysqli();
    $db->open($config);
    return $db;
}
function close_database($db){
    $db->close();
}

/*动态拼装表头*/
function table_head(){
        $str="<thead><tr>";
        $varArray=func_get_args();
        foreach($varArray as $value) {
          $str .=  "<th>" .$value . "</th>\n";
        }
        $str.="</tr></thead>\n";
        return $str;
}

/*动态拼装表内容*/
function table_body(){
    $str="<tr>";
    $varArray=func_get_args();
    foreach($varArray as $value) {
        $str .=  "<td>" .$value . "</td>\n";
    }
    $str.="</tr>\n";
    return $str;
}

function get_table_style(){
 return "<table class='table table-bordered table-striped table-hover table-condensed'>";
}

/*共公检查类函数*/
function public_check(){
//   return isset($_POST["oper"]);
   return isset($_POST["id"]);
}

/*拼一个共公的删除Click事件*/
function get_delete_button($str, $id)
{
	$butt_del='<button onClick="delPubfunction(';
	$butt_del .= "'".$str."',";
	$butt_del .="'".$id."'";
	$butt_del .=');">删除</button>';
	return $butt_del;
}

/*调用公共插入语句*/
function public_insert($db, $table){
    $fields=$db->get_fields($table);
    $k=$db->get_primary($table);
    $keys=array_keys($fields);
    //print_r($keys);
    $data=array();
    foreach ($keys as $key => $value) {
        if($k===$value){
            //print_r($k);
            continue;
        }
        if($value==="datetime"){
            $data[datetime]=date("Y-m-d h:i:sa");
        }else if($value==="reg_date"){
            $data[reg_date]=date("Y-m-d");
        }else{
            if($_POST["$value"] != "") {
                $data[$value]= $_POST[$value];
            }
        }
    }
    //print_r($data);
    return $db->insert($data, $table);
}

/*调用公共插入语句*/
function public_delete($db, $table){
    $key=$db->get_primary($table);
    //print_r($keys);
    if($table==="p_acctreg")
    {
        $regid=$_POST["regid"];
        $datetime=date("Y-m-d h:i:sa");
        $sql="update p_acctreg set is_valid='N',datetime='$datetime' where regid=$regid ";
        return $db->query($sql);
    }else{
        $where=$key .'='. $_POST[$key];
    }

    //print_r($where);
    return $db->delete($table, $where);
}

/*调用公共新增语句*/
function public_update($db, $table){
    $fields=$db->get_fields($table);
    $k=$db->get_primary($table);
    $keys=array_keys($fields);
    //print_r($keys);
    $data=array();
    foreach ($keys as $key => $value) {
        if($k===$value){
            $where=$k."=".$_POST[$value];
            continue;
        }
        if($value==="datetime"){
            $data["datetime"]=date("Y-m-d h:i:sa");
        }else{
            if($_POST["$value"] != "") {
                $data[$value]=$_POST[$value];
            }
        }
    }
    //print_r($where);
    //print_r($data);
    return $db->update($data, $table, $where);
}

/**单表的无条件查询返回**/
function public_select($db, $table1){
    $table='p_'.$table1;
    $k=$db->get_primary($table);
    $cols=$db->get_table_cols($table);
    $sql ="select * from ".$table;

    $name=$_POST["baby_name"];
    if($table=="p_acctreg"){
        $bgndate=$_POST["bgndate"];
        $sql.=" where 1=1 ";
        if($bgndate!=""){
            $sql.= "and acct_date>='$bgndate' ";
        }
        $enddate=$_POST["enddate"];
        if($enddate!=""){
            $sql.= "and acct_date<='$enddate' ";
        }
    } else if($name!=""){
        $sql .= " where baby_name like '%$name%' ";
    }
    $sql .= ' order by '.$k.' desc';
// echo "<p>".$sql."</p>";

    //处理表头
    echo get_table_style();
    echo "<tr>";
    $i=0;
    foreach ($cols as $key => $value) {
        echo '<th>'."$value".'</th>';
        $i+=1;
    }
    echo '<th>操作</th><th>操作</th></tr>';
    $results=$db->query($sql);
    while($row=$db->fetch_next()) {
        echo "<tr>";

        $button='<button onClick="tablePubClick('."'".$table1."'".','.$i.','.$row[$k]. ');">修改</button>';
        $butt_del='<button onClick="delPubfunction(';
        $butt_del .= "'id=delete_". $table1. "&". $k. "=".$row[$k] ."'". ');">';
        $butt_del .= '删除</button>';
        foreach ($cols as $key => $value) {
            echo "<td>".$row[$key]."</td>";
        }
        echo '<td>'.$button.'</td>';
        echo '<td>'.$butt_del.'</td>';
        echo '</tr>';
    }
    echo "</table>";
    return true;
}
/**分发**/
function public_router( ){
    $ret==false;
    $idstr=$_POST['id'];
    $token=strtok($idstr, '_');
    $oper=$token;
    $table1=strtok('_');
    $table='p_'.$table1;

$db=open_database();

    // $db->query("set names utf8");
//echo $oper.'-'.$table.'<br />';
    if($oper==="insert"){
        $ret=public_insert($db, $table);
    }else if($oper==="delete"){
        $ret=public_delete($db, $table);
    }else if($oper==="update"){
        $ret=public_update($db, $table);
    }else if($oper==="select"){
        $ret=public_select($db, $table1);
    }else {
        $params=array( );
        $params[]=$db;
        $ret=call_user_func_array($idstr, $params);
    }

    close_database($db);

    if($oper==="insert" ||
       $oper==="delete" ||
       $oper==="update"){
        if($ret){
            echo "处理成功!";
        }else{
            echo $db->error();
            return false;
        }
    }
    return true;
}
