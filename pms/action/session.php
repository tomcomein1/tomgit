<?php

function set_session_info($user, $auth){
  $lifeTime = 24 * 3600;
  session_set_cookie_params($lifeTime);
  session_start();

  $_SESSION['user'] = $user;
  $_SESSION['auth'] = $auth;
}


function login($db) {
   $user_phone=$_POST['user_phone'];
   $user_pass=$_POST['user_pass'];

   if ($user_phone=="" || $user_pass=="") {
      echo "您的用户电话或密码为空，请重新输入!";
      return false;
   }

   $sql="select * from p_user 
          where user_phone='$user_phone' ";
  

   $db->query($sql);
   if( $db->querycount > 0) {
     
   } else {
      echo "您不是合法的用户电话:".$user_phone;
      return false;
   }
   while($row=$db->fetch_next() ) {
      if ($row['user_pass'] === $user_pass) {
         set_session_info($row['user_name'], $row['user_auth']);
	 echo $row['user_name']."欢迎您的到来";
	 return true;
      }
   }
   echo "您的密码不正确，请重新输入";
   return false;
}

function logout(){
    session_start();
    $name=$_SESSION['user'];
    if ($name=="") {
       echo "您还没有登录，无需注销!";
       return false;
    }

    // 这种方法是将原来注册的某个变量销毁
    unset($_SESSION['user']);
    unset($_SESSION['auth']);
    // 这种方法是销毁整个 Session 文件
    session_destroy();
    echo $name.",您已注销成功,谢谢使用!";
}

