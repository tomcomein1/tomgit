<?php

/*查询*/
function sel_cust_info($db){
    $baby_name=$_POST["baby_name"];
    $sql="select * from p_custom ";
    if ($baby_name!=""){
        $sql .= "where baby_name like '%$baby_name%' ";
    }

    $sql.="order by custid desc";

//echo $sql;

    echo get_table_style();
    echo table_head("客户编号", "宝宝姓名", "宝宝生日", "宝宝小名", "宝贝性别",
                    "家长电话", "家长姓名", "家长qq号", "渠道来源", "客户地址", 
		    "会员编号", "备注信息", "功能");
    $results=$db->query($sql);

    while($row=$db->fetch_next()) {
        $button='<button onClick="tableInsertClick('."'booking',2,".$row[custid]. ');">预约</button>';
        echo table_body(
	$row['custid'], $row['baby_name'], $row['baby_birth'], $row['nick_name'], $row['baby_sex'],
	$row['cust_phone'], $row['cust_name'], $row['qq'], $row['chnl'], $row['addr'],
        $row['mb_no'], $row['memo'], $button);
    }
    echo "</table>";
    return true;
}


function sel_booking($db){
    $sql="select * from p_booking a
          left join p_custom b
	         on a.custid=b.custid ";

    $sql.="order by bookid desc";

//echo $sql;

    echo get_table_style();
    echo table_head("预约编号", "预约拍摄日期", "预约拍摄时间", "预约套系", "渠道类型",
		"预约拍摄方式", "客户编号", "订单编号", "预约备注", "修改时间", "宝宝名称",
		"家长电话", "功能", "功能");
    $results=$db->query($sql);

    while($row=$db->fetch_next()) {
        $button='<button onClick="tablePubClick('."'booking',12,".$row['bookid']. ');">修改</button>';
        $butt_del='<button onClick="delPubfunction(';
        $butt_del .= "'id=delete_booking&bookid=".$row['bookid'] ."'". ');">';
        $butt_del .= '删除</button>';
        echo table_body($row['bookid'], $row['book_date'], $row['book_time'],$row['suit'],$row['chnl'],
             $row['method'], $row['custid'], $row['order_no'], $row['book_memo'], $row['datetime'], $row['baby_name'],
             $row['cust_phone'], $button, $butt_del);
    }
    echo "</table>";
    return true;
}
