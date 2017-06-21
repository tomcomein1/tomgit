<?php
  header('Content-Type: text/event-stream');
  header('Cache-Control: no-cache');

  $time = date('Ymd h:i');
  echo $time;
  flush();
?>
