<?php
    //演示下载图片
$file_name="Image58.png";
    //1.打开文件
if(!file_exists($file_name)){
    echo "文件不存在";
    return;
}

// $fp=fopen($file_name,"r");
    //获取下载文件的大小
$file_size=filesize($file_name);
var_dump($file_size);
?>