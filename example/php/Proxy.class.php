<?php
/*抽象主题角色（IGiveGift）：定义了Follower和Proxy公用接口，这样就在任何使用Follower的地方都可以使用Proxy。
       主题角色（Follower）：定义了Proxy所代表的真实实体。
       代理对象（Proxy）：保存一个引用使得代理可以访问实体，并提供一个与Follower接口相同的接口，这样代理可以用来代替实体(Follower)。
*/

interface IGiveGift{
    function giveRose();
    function giveChocolate();
}

/**追求者
*class Follower
*/
class Follower implements IGiveGift{
    private $girlName;
    function __construct($name='Girl'){
        $this->girlName=$name;
    }
    function giveRose(){
        echo "{$this->girlName}:这是我送你的玫瑰，希望你能喜欢。\n";
    }
    function giveChocolate(){
        echo "{$this->girlName}:这是我送你巧克力，希望你能喜欢。\n";
    }
}

class Person{
    function sayThanks(){
        echo "谢谢你，这是给你的酬劳。\n";
    }
}
/**代理
*class Proxy
*/
class Proxy implements IGiveGift{
    private $follower;
    private $person;
    function __construct($name='Girl'){
        $this->prepareWork();
        $this->follower=new Follower($name);
    }
    function prepareWork(){
        echo "做这事之前，我需要先准备一下，很紧张。\n";
    }
    function giveRose(){
        $this->follower->giveRose();
    }
    function giveChocolate(){
        $this->follower->giveChocolate();
    }
    function __destruct(){
        $this->person=new Person();
        $this->person->sayThanks();
    }
}

// $proxy=new Proxy('小花');
// $proxy->giveRose();
// $proxy->giveChocolate();
/**
 * Record类
 */
class Record
{
    /**
     * @var array|null
     */
    protected $data;

    /**
     * @param null $data
     */
    public function __construct($data = null)
    {
        $this->data = (array) $data;
    }

    /**
     * magic setter
     *
     * @param string $name
     * @param mixed  $value
     *
     * @return void
     */
    public function __set($name, $value)
    {
        $this->data[(string) $name] = $value;
    }

    /**
     * magic getter
     *
     * @param string $name
     *
     * @return mixed|null
     */
    public function __get($name)
    {
        if (array_key_exists($name, $this->data)) {
            return $this->data[(string) $name];
        } else {
            return null;
        }
    }
}
/**
 * RecordProxy类
 */
class RecordProxy extends Record
{
    /**
     * @var bool
     */
    protected $isDirty = false;

    /**
     * @var bool
     */
    protected $isInitialized = false;

    /**
     * @param array $data
     */
    public function __construct($data)
    {
        parent::__construct($data);

        // when the record has data, mark it as initialized
        // since Record will hold our business logic, we don't want to
        // implement this behaviour there, but instead in a new proxy class
        // that extends the Record class
        if (null !== $data) {
            $this->isInitialized = true;
            $this->isDirty = true;
        }
    }

    /**
     * magic setter
     *
     * @param string $name
     * @param mixed  $value
     *
     * @return void
     */
    public function __set($name, $value)
    {
        $this->isDirty = true;
        parent::__set($name, $value);
    }
}
class ProxyTest extends Record
{
    public function testSetAttribute(){
        $data = [];
        $proxy = new RecordProxy($data);
        $proxy->xyz = false;
        $this->assertTrue($proxy->xyz===false);
    }
}

$pt=new ProxyTest();