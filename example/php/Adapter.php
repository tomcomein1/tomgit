<?php

interface PaperBookInterface{
    /**
    *翻页方法
    */
    public function turnPage();
    /*
    *打开书的方法
    */
    public function open();
}

class Book implements PaperBookInterface{
    public function open(){
        echo "打开书\n";
    }
    public function turnPage(){
        echo "向后翻\n";
    }
}

class EBookAdapter implements PaperBookInterface{
    protected $eBook;
    public function __construct(EBookInterface $ebook){
        $this->eBook=$ebook;
    }
    public function open(){
        $this->eBook->pressStart();
    }
    public function turnPage(){
        $this->eBook->pressNext();
    }
}

interface EBookInterface{
     public function pressNext();
     public function pressStart();
}
/**
 * Kindle 是电子书实现类
 */
class Kindle implements EBookInterface
{
    /**
     * {@inheritdoc}
     */
    public function pressNext()
    {
        echo "我是电子书后翻\n";
    }

    /**
     * {@inheritdoc}
     */
    public function pressStart()
    {
        echo "我是电子书开始阅读\n";
    }
}

/**
 * AdapterTest 用于测试适配器模式
 */
class AdapterTest {
    /**
     * @return array
     */
    public function getBook()
    {
        return array(
            array(new Book()),
            // 我们在适配器中引入了电子书
            array(new EBookAdapter(new Kindle()))
        );
    }

    /**
     * 客户端只知道有纸质书，实际上第二本书是电子书，
     * 但是对客户来说代码一致，不需要做任何改动
     *
     * @param PaperBookInterface $book
     *
     * @dataProvider getBook
     */
    public function testIAmAnOldClient(PaperBookInterface $book)
    {
        $this->assertTrue(method_exists($book, 'open'));
        $this->assertTrue(method_exists($book, 'turnPage'));
    }
}

$ap=new AdapterTest();