<?php

/*查询*/
function sel_photo_custinfo($db){

    $baby_name=$_POST["baby_name"];

    $sql="select a.* ,b.* from p_booking a
       left join p_custom b on a.custid = b.custid
	   where not exists(
		  select 1 from p_photography c
		   where a.bookid=c.bookid) ";

    if ($baby_name!="") {
        $sql .=" and baby_name like '%$baby_name%' ";
    }
    $sql .= "order by a.bookid desc";

    //echo $sql;
    echo get_table_style();
    echo table_head("客户编号", "预约编号", "订单编号", "宝宝姓名", "宝宝性别", "宝宝小名",
             "套系", "家长姓名", "家长电话", "宝贝生日", "操作方式");
    $results=$db->query($sql);

    while($row=$db->fetch_next()) {
        $button='<button onClick="tableInsertClick('."'photography',4,".$row[custid]. ');">登记拍摄</button>';
        $sex=($row['baby_sex']=="1") ? "男" : "女";

        echo table_body( $row[custid], $row[bookid], $row[order_no], $row[baby_name], $sex, $row[nick_name],
	     $row['suit'], $row[cust_name], $row[cust_phone], $row[baby_birth], $button);
    }
    echo "</table>";
    return true;
}

/*查询*/
function sel_photography($db){

    $cust_name=$_POST["cust_name"];

    $sql="select a.* ,b.* from p_photography a
       left join p_custom b
        on a.custid = b.custid ";

    $sql .= "order by a.photoid desc";

    //echo $sql;
    echo get_table_style();
    echo table_head("拍摄编号", "拍摄日期", "拍摄人", "拍摄备注", "预约编号",  "客户编号", "订单编号",
                                "修改日期", "宝宝姓名", "家长姓名", "家长电话", "功能", "功能");
    $results=$db->query($sql);

    while($row=$db->fetch_next()) {
        $button='<button onClick="tablePubClick('."'photography',11,".$row[photoid]. ');">修改</button>';
        $butt_del='<button onClick="delPubfunction(';
        $butt_del .= "'id=delete_photography&photoid=".$row['photoid'] ."'". ');">';
        $butt_del .= '删除</button>';

        echo table_body( $row[photoid], $row[photo_date], $row[photo_man], $row[photo_memo], $row[bookid],
        $row[custid], $row[order_no], $row[datetime], $row[baby_name],$row[cust_phone], $row[baby_name],
        $button, $butt_del);
    }
    echo "</table>";
    return true;
}
