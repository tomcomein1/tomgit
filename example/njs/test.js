Function.prototype.method=function(name, func){
    if(!this.prototype[name]) {
        this.prototype[name]=func;
    }
    return this;
};

var test=function(){
    var str='hello world';
    return{
        at:function(){
            var names=['david', 'cyaf', 'safs', 'clay', 'Jesafra'];
            var name='david';
            var position=names.indexOf(name);
            if(position>=0) {
                console.log('fond ' + name + ' at position ' + position);
            } else {
                console.log(name + ' not found in array');
            }
        },
        sayHello:function(){
            return str;
        },
        List:function(){
            return str;
        }
    };
};

var t=test();
// t.at();
// console.log(t.List());
//哈希
var hashMap={
    len:0,
    Set:function(key, value){this.len++; this[key]=value;},
    Get:function(key){if(this.len>0) return this[key]; else return null;},
    Contains:function(key){return this.Get(key)===null ? false:true;},
    Remove:function(key){this.len--; return delete this[key];},
    length:function() {return this.len;}
};

// console.log(hashMap.Set('name', 'John Smith'));
// console.log(hashMap.Set('age', 24));
// console.log(hashMap.Get('name'));
// console.log(hashMap.Contains('title'));
// console.log(hashMap.Remove('age'));
// console.log(hashMap.Remove('name'));
// console.log(hashMap.length());
// console.log(hashMap.Get('name'));

Function.method('new', function(){
    //创建一个新对象,它继承自构造器的原型对象。
    var that=Object.create(this.prototype);
    //调用构造器函数，绑定-this-到新对象上
    var other=this.apply(that, arguments);
    //如果它的返回值不是一个对象，就返回该新对象
    return (typeof other==='object' && other) || that;
});

//伪类
var mammal=function(spec){
    var that={};
    that.get_name=function(){
        return spec.name;
    };
    that.says=function(){
        return spec.saying || '';
    };
    return that;
};
var myMammal=mammal({name:'Herb', saying:'hello'});
console.log(myMammal.get_name() +' ' +  myMammal.says());
var cat=function(spec){
    spec.saying=spec.saying || 'meow';
    var that=mammal(spec);
    that.purr=function(){
        return 'rr';
    };
    that.get_name=function(){
        return this.purr() + ' ' + spec.name + ' '+ that.says();
    };
    return that;
};
var myCat=cat({name:'herrab'});
console.log(myCat.get_name());
Object.method('superior',function(name){
    var that=this,
    method=that[name];
    return function(){
        return method.apply(that, arguments);
    };
});

var coolcat=function(spec){
    var that=cat(spec);
    super_get_name=that.superior('get_name');
    that.get_name=function(n){
        return 'like ' + super_get_name() + ' baby';
    };
    return that;
};
var myCoolCat=coolcat({name: 'Bix'});
console.log(myCoolCat.get_name());

var eventuality=function(that){
    var registry={};
    that.fire=function(event){
        //在一个对象上触发一个事件。该事件可以是一个包含事件名称的字符串，
        //或者是一个拥有包含事件名称的type属性的对象。
        //通过'on'方法注册的事件处理程序中匹配事件名称的函数将被调用。
        var array,
            func,
            handler,
            i,
            type=typeof event==='string' ? event : event.type;
        //如果这个事件存在一组事件处理程序，那么就遍历它们并按顺序依次执行。
        if(registry.hasOwnProperty(type)){
            array=registry[type];
            for(i=0; i<array.length; i+=1) {
                handler=array[i];
                //每个处理程序包含一个方法和一组可选的参数
                //如果该方法是一个字符串形式的名字，那么寻找到该函数.
                func=handler.method;
                if(typeof func==='string'){
                    func=this[func];
                }
                //调用一个处理程序。如果该条目包含参数，那么传递它们过去。否则，传递该事件对象。
                func.apply(this, handler.parameters || [event]);
            }
        }
        return this;
    };
    that.on=function(type, method, parameters){
        //注册一个事件。构造一条处理程序条目，将它插入到处理程序数组中，
        //如果这种类型的事件还不存在，就构造一个。
        var handler={
            method:method,
            parameters:parameters
        };
        if(registry.hasOwnProperty(type)){
            registry[type].push(handler);
        } else{
            registry[type] = [handler];
        }
        return this;
    };
    return that;
};

eventuality(myCoolCat);
myCoolCat.health = 100;
myCoolCat.on('dead', function (event) {
    var date = event.date;
    var killer = event.killer;
    console.log("%s killed me at %s", killer, date);
});
myCoolCat.on('hurt', function () {
    this.health -= 50;
    if (this.health <= 0) {
        var event = {
            type: 'dead',
            killer: 'stone from sky',
            date: new Date()
        };
        this.fire(event);
    } else {
        console.log('nothing, who is the strong guy.');
    }
});

// myCoolCat.fire("hurt"); //nothing, who is the strong guy.
// myCoolCat.fire("hurt");
var window={};
window.firstName = "diz";
window.lastName = "song";
var myObject = { firstName: "my", lastName: "Object" };
function HelloName() {
　　console.log("Hello " + this.firstName + " " + this.lastName, " glad to meet you!");
}
HelloName.call(window); //huo .call(this);
HelloName.call(myObject);
function sum(num1, num2) {
    return num1 + num2;
}
console.log(sum.call(window, 10, 10)); //20
console.log(sum.apply(window,[10,20])); //30
window.color='red';
var o={color:'blue'};
function sayColor(){
    console.log(this.color);
}
var OSayColor=sayColor.bind(o);
OSayColor();

Array.method('reduce', function(f,value){
    var i;
    for(i=0; i<this.length; i+=1){
        value=f(this[i], value);
    }
    return value;
});
var add=function(a, b){
    return a+b;
};
var mult=function(a, b){
    return a*b;
};

var data=[4, 8, 15, 17, 23, 42];
var sum=data.reduce(add, 0);
var product=data.reduce(mult, 1);
// console.log('sum=' + sum , 'product=' + product);

var $=function(value){
    var now=new Date();
    console.log(now.getFullYear(), now.getMonth(), now.getDay(), '$ function show :' , value);
};

$('wo qu');