<?php
    $dbhost = 'localhost';
    // mysql用户名
    $dbuser = 'root';
    // mysql用户名密码
    $dbpass="root";
    //数据库连接
    $conn = mysqli_connect($dbhost, $dbuser, $dbpass);
    if(!$conn){
        echo "数据库连接失败". mysqli_error($conn);
    }else{
        echo '连接成功<br />';
    }


    $sql='CREATE DATABASE RUNOOT';
    $retval = mysqli_query($conn,$sql);
    if(!$retval){
        echo "数据库创建失败";
    }else{
        echo "数据库创建成功";
    }

    //关闭数据库连接
    mysqli_close($conn);
 ?>