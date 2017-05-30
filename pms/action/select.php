<?php
include_once "./db.pub.php";
include_once "./report.php";

product_select();
/*取得渠道类型代码*/
function product_select(){
/*打开数据库连接*/
    $db=open_database();

    $oper=$_GET["oper"];
    if($oper=="chnl"){
        chnl_select($db);
    } else if($oper=="suit") {
        suit_select($db);
    } else if($oper=="method") {
        method_select($db);
    } else if($oper=="todaywarn"){
        todaywarn_select($db);
    }else{
        echo "无效的操作符!";
    }
    /*关闭数据库连接*/
    return close_database($db);
}

function chnl_select($db){
    $chnl=$_GET["chnl"];
    $value=$_GET["value"];
    $sql="select * from p_channel";

    $db->query($sql);
    echo "<select name='chnl'>";
    while($row=$db->fetch_next()) {
        if($value==$row['chnl']){
            echo "<option selected='selected' value={$row['chnl']}>{$row['chnl']}-{$row['chnl_name']}</option>";
        }
        else {
            echo "<option value={$row['chnl']}>{$row['chnl']}-{$row['chnl_name']}</option>";
        }
    }
    echo "</select>";
}

function method_select($db){
    $method=$_GET["method"];
    $name=$_GET["name"];
    $value=$_GET["value"];
    $sql="select * from p_method";

    $db->query($sql);
    echo "<select name='".$name."'>";
    while($row=$db->fetch_next()) {
        if($value==$row['method']){
            echo "<option selected='selected' value={$row['method']}>{$row['method']}-{$row['method_name']}</option>";
        }
        else {
            echo "<option value={$row['method']}>{$row['method']}-{$row['method_name']}</option>";
        }
    }
    echo "</select>";
}

function suit_select($db){
    $suit=$_GET["suit"];
    $value=$_GET["value"];
    $sql="select * from p_suit";

    $db->query($sql);
    echo "<select name='suit'>";
    while($row=$db->fetch_next()) {
        if($value==$row['suit']){
            echo "<option selected='selected' value={$row['suit']}>{$row['suit']}-{$row['suit_name']}</option>";
        }
        else {
            echo "<option value={$row['suit']}>{$row['suit']}-{$row['suit_name']}</option>";
        }
    }
    echo "</select>";
}

function todaywarn_select($db){
    $today=date("Y-m-d");
    $tomorrow=date("Y-m-d", strtotime("$today+1 days"));
    get_days_info($db, $today, $tomorrow);

    echo "<h2>最近30天过生日的宝宝</h2>";
    birthquery($db);
}

/*提醒函数*/
function get_days_info($db, $bgndate, $enddate){
    echo "<p class='wanning'>已预约拍摄客户:";
    $sql="select * from p_booking a
        left join p_custom b 
	  on a.custid=b.custid
         where a.book_date>='$bgndate'
           and a.book_date<='$enddate'
	   and not exists
	   ( select 1 from p_photography c where a.bookid=c.bookid)
         order by a.book_date, a.bookid";
// echo $sql;
    $db->query($sql);
    while($row=$db->fetch_next()){
        echo "[".$row[book_date]."]===>";
        echo "宝宝姓名-".$row[baby_name].", ";
        echo "家长电话-".$row[cust_phone].", ";
        echo "预约时间-".$row[book_date]." ".$row[book_time]."<br />";
    }
    echo "</p>";

    echo "<p class='wanning'>选片客户:";
    $sql="select * from p_order a
        left join p_photography b
          on a.order_no=b.order_no
        left join p_custom c
          on b.custid=c.custid
         where substring(a.cp_time,1,8) >= '$bgndate'
           and substring(a.cp_time,1,8) <= '$enddate'
         order by a.cp_time, a.orderid";
 //echo $sql;
    $db->query($sql);
    while($row=$db->fetch_next()){
        echo "[".$row[cp_date]."]===>";
        echo "宝宝姓名-".$row[baby_name].", ";
        echo "家长电话-".$row[cust_phone].", ";
        echo "初修完成时间-".$row[order_date]."<br />";
    }
    echo "</p>";
}
