<?php
/**回调**/

include_once "./db.pub.php";
include_once "./regbook.php";
include_once "./regphoto.php";
include_once "./report.php";

if (public_check()==false ) {
   echo "你无权操作该功能.";
   return false;
}

public_router( );
