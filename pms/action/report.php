<?php
//include_once "libs/classes/db_mysqli.class.php";
include_once "./db.pub.php";

function custquery($db){
    $custname=$_POST["custname"];

    $sql="select * from p_custom a
        left join p_booking b
          on a.custid = b.custid
        left join p_photography c
          on b.custid = c.custid ";
    if($custname!=""){
        $sql.="where a.cust_name like '%$custname%' ";
    }
    $sql .= "order by a.custid desc";

// echo $sql;
    echo get_table_style();
    echo table_head( "宝贝名称", "宝贝生日", "宝贝性别", "家长名称", "家长电话",
		"预约日期", "套系", "拍摄日期", "拍摄人", "渠道");
    $results=$db->query($sql);

    while($row=$db->fetch_next()) {
        if($row['baby_sex']=="0")
            $sex="女";
        else $sex="男";
        if($row['book_date']==""){
            $book_date="未预约";
        } else {
            $book_date=$row['book_date'];
        }
        if($row['photo_date']==""){
            $photo_date="未拍摄";
        } else {
            $photo_date=$row['photo_date'];
        }

        echo table_body( $row['baby_name'],$row['baby_birth'], $sex, $row['cust_name'], $row['cust_phone'],
	     $book_date, $row['suit'], $photo_date ,$row['photo_man'], $row['chnl'] );
    }
    echo "</table>";
}

function acctquery($db){
    $bgndate=$_POST["bgndate"];
    $enddate=$_POST["enddate"];

    $sql = "select * from p_acctreg where 1=1 ";
    if($bgndate!=""){
        $sql .= "and acct_date>='$bgndate' ";
    }
    if ($enddate!=""){
        $sql .= "and acct_date<='$enddate' ";
    }
    $sql .= "order by regid ";

    echo get_table_style();
    echo table_head("账务编号","账务日期","借贷标志", "发生额", "收支方式", "收付款人", "登记人",
                                "操作员", "备注", "是否有效" );
    $dsum=0.00;
    $csum=0.00;
    $results=$db->query($sql);
    while($row=$db->fetch_next()) {
        if($row['dr_cr']=="1") {
            if($row['is_valid']=='Y') {
                $valid="有效";
                $dsum += $row[acct_amt];
            }
            else $valid="无效";
        }
        else {
            $dcflag="收入";
            if($row['is_valid']=='Y') {
                $valid="有效";
                $csum += $row[acct_amt];
            }
            else $valid="无效";
        }

        echo table_body($row['regid'],$row['acct_date'], $dcflag, $row['acct_amt'], $row['pay_mode'],
  	                $row['pay_man'], $row['reg_man'], $row['user_phone'], $row['reg_memo'], $valid);
    }

    echo table_body("合计:", "-", "总收入:", $csum."元", "总支出:", $dsum."元", "利润:", ($csum-$dsum)."元", "-", "-");
    echo "</table>";
    return true;
}

//已拍摄客户查询
function photoquery($db){
    $cust_name=$_POST["cust_name"];

    $sql="select photo_date, photo_man, cust_name,cust_phone,baby_name,baby_birth,baby_sex
        from p_photography A, p_custom B
        where A.custid=B.custid ";
    if($cust_name!=""){
        $sql .= " and B.cust_name like '%$cust_name%' ";
    }
    $sql .=" order by photoid desc ";
    //echo $sql;

    echo get_table_style();
    echo table_head( "宝贝名称", "宝贝生日", "宝贝性别", "家长名称", "家长电话",
                     "拍摄日期", "拍摄人");
    $results=$db->query($sql);

    while($row=$db->fetch_next()) {
        if($row['baby_sex']=="0")
            $sex="女";
        else $sex="男";
        echo table_body(
	     $row['baby_name'], $row['baby_birth'], $sex, $row['cust_name'], $row['cust_phone'],
	     $row['photo_date'],$row['photo_man'] );
    }
    echo "</table>";
}

//预约且未拍摄客户查询
function bookquery($db){
    $cust_name=$_POST["cust_name"];

    $sql= "select a.* ,b.* from p_booking a
               left join p_custom b on a.custid = b.custid
	    where not exists(
	              select 1 from p_photography c
		       where a.bookid=c.bookid) ";
    if($cust_name!=""){
        $sql .= " and B.cust_name like '%$cust_name%' ";
    }
    $sql .=" order by A.bookid desc ";
    // echo $sql;

    echo get_table_style();
    echo table_head("预约日期", "预约时间", "套系", "渠道","订单编号", "家长名称",
                    "家长电话", "宝贝名称", "宝贝生日", "宝贝性别", "客户备注");
    $results=$db->query($sql);

    while($row=$db->fetch_next()) {
        if($row['baby_sex']=="0")
            $sex="女";
        else $sex="男";
        echo table_body( $row['book_date'], $row['book_time'], $row['suit'], $row['chnl'],$row['order_no'],
            $row['cust_name'], $row['cust_phone'], $row['baby_name'],$row['baby_birth'], $sex, $row['memo'] );
    }
    echo "</table>";
}

function birthquery($db) {

    $befor_days=$_POST["befor_days"];

    $currdate=date("Y-m-d");
    if($befor_days=="") {
       $befor_days=30;
    }
    $date=date("Y-m-d", strtotime("$currdate + $befor_days days"));
    //echo "$befor_days"."$date";

    $sql="select * from ( p_custom a
        left join p_booking b
          on a.custid = b.custid )
        left join p_photography c
          on b.custid = c.custid ";
    $sql.="where
        concat(substring(now(),1,4),'-',substring(baby_birth,6)) >= '$currdate'
        and concat(substring(now(),1,4),'-',substring(baby_birth,6)) <= '$date'
        order by a.custid desc";

// echo $sql;
    echo get_table_style();
    echo table_head( "宝贝名称", "宝贝生日", "宝贝性别", "家长名称", "家长电话",
		     "预约日期", "套系", "渠道", "拍摄日期", "拍摄人");
    $results=$db->query($sql);

    while($row=$db->fetch_next()) {
        if($row['baby_sex']=="0")
            $sex="女";
        else $sex="男";
        if($row['book_date']==""){
            $book_date="未预约";
        } else {
            $book_date=$row['book_date'];
        }
        if($row['photo_date']==""){
            $photo_date="未拍摄";
        } else {
            $photo_date=$row['photo_date'];
        }

        echo table_body(
	$row['baby_name'], $row['baby_birth'], $sex, $row['cust_name'], $row['cust_phone'],
	$book_date,$row['suit'], $row['chnl'], $photo_date ,$row['photo_man'] );
    }
    echo "</table>";
}

//预约且未拍摄客户查询
function orderquery($db){
    $order_no=$_POST["order_no"];

    $method=array();
    $result=$db->query("select * from p_method");
    while($rows=$db->fetch_next()){
       $method[$rows['method']]=$rows['method_name'];
    }

    $sql= "select a.*, b.photo_date,c.baby_name,c.cust_phone,c.chnl, d.book_date 
        from p_order a
        left join p_photography b
          on a.order_no = b.order_no
        left join p_custom c
          on b.custid = c.custid
        left join p_booking d
          on c.custid=d.custid";
    if($order_no!=""){
        $sql .= " where a.order_no like '%$order_no%' ";
    }
    $sql .=" order by a.orderid desc ";
 //    echo $sql;

    echo get_table_style();
    echo table_head("订单编号", "宝宝姓名","家长电话", "渠道", "套系",
                    "预约日期", "拍摄日期", "订单日期", "初修日期", "初修人",
		    "选片方式", "选片时间", "二销内容", "精修日期","精修人",
		    "交件日期", "回件方式", "回件情况", "取件时间", "取件方式");
    $results=$db->query($sql);


    while($row=$db->fetch_next()) {
        foreach ($method as $mtd => $name) {
            if($mtd==$row[cp_method]){
                $cp_name=$name;
            }
            if($mtd==$row[ret_method]){
                $ret_name=$name;
            }
            if($mtd==$row[get_method]){
                $get_name=$name;
            }
        }

        echo table_body( $row['order_no'], $row['baby_name'],$row['cust_phone'], $row['chnl'], $row['suit'],
	    $row['book_date'], $row['photo_date'], $row['order_date'], $row['first_date'], $row['first_man'],
	    $cp_name, $row['cp_time'], $row['cp_cont'], $row['second_date'], $row['second_man'],
            $row['pm_date'], $ret_name, $row['ret_circs'],  $row['get_photo'], $get_name );
    }
    echo "</table>";
}
