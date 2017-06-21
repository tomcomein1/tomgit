<?php
$username='jellybool';
$password='jellybool.com';
/*简单地对字符串进行md5加密*/
echo md5($username);
echo "<hr>\n";
echo md5($password);
echo "<hr>\n";
/*更推荐的做法是对重要的敏感数据进行多次加密，以防被轻易破解*/
echo md5(md5($password));
echo "<hr>\n";
echo crypt($password,"jellybool");
echo "<hr>\n";
echo sha1($password);
echo "\n";
