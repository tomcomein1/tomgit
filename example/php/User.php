<?php
class User{
    protected $username='';
    public function __construct($username){
        $this->username=$username;
    }
    public function getProfilePage(){
        $profile="<h2>I like Never Again!</h2>\n";
        $profile.="I love all of their songs.My favorite CD:<br />\n";
        $profile.="{{myCD.getTitle}}!!\n";
        return $profile;
    }
}

class userCD{
    protected $user=NULL;
    public function setUser($user){
        $this->user=$user;
    }
    public function getTitle(){
        $title='Waste of a Rib';
        return $title;
    }
}
class userCDInterpreter{
    protected $user=NULL;
    public function setUser($user){
        $this->user=$user;
    }
    public function getInterpreted(){
        $profile=$this->user->getProfilePage();
        if(preg_match_all('/\{\{myCD\.(.*?)\}\}/', $profile,
            $triggers, PREG_SET_ORDER)){
            $replacements = array();
            foreach ($triggers as $trigger) {
                $replacements[]=$trigger[1];
            }
            $replacemetns=array_unique($replacements);
            $myCD = new userCD();
            $myCD->setUser($this->user);
            foreach ($replacements as $replacement) {
                $profile=str_replace("{{myCD.{$replacement}}}",
                    call_user_func(array($myCD, $replacement)), $profile);
            }
        }
        return $profile;
    }
}

$username='aaron';
$user=new User($username);
$interpreter=new userCDInterpreter();
$interpreter->setUser($user);
print "<h1>{$username}'s Profile</h1>\n";
print $interpreter->getInterpreted();