Function.prototype.method=function(name, func){
    if(!this.prototype[name]) {
        this.prototype[name]=func;
    }
    return this;
};

Function.method('new', function(){
    //创建一个新对象,它继承自构造器的原型对象。
    var that=Object.create(this.prototype);
    //调用构造器函数，绑定-this-到新对象上
    var other=this.apply(that, arguments);
    //如果它的返回值不是一个对象，就返回该新对象
    return (typeof other==='object' && other) || that;
});
Object.method('superior',function(name){
    var that=this,
    method=that[name];
    return function(){
        return method.apply(that, arguments);
    };
});

Number.method('integer', function(){
        return Math[this<0 ? 'ceil' : 'floor'](this);
});

// console.log((-10/3).integer()); //-3
//字符串删除前后空格
String.method('trim', function(){
        return this.replace(/^\s+|\s+$/g,'');
});
// console.log('"' + "    neat   ".trim() + '"');
String.method('charAt', function(pos)){
    return this.slice(pos, pos+1);
};

var fade=function(node){
        var level=1;
        var step=function(){
                var hex=level.toString(16);
                node.style.backgroundColor='#FFFF' + hex + hex;
                if(level<15) {
                        level+=1;
                        setTimeout(step, 100);
                }
        };
        setTimeout(step, 100);
};
//fade(document.body);
var add_the_handlers=function(nodes){
        var click=function(element){
            return function(e) {
                //do_someting
            };
        };
        var i;
        for(i=0; i<nodes.length; i+=1){
                nodes[i].onclick=click(nodes[i]);
        }
};

//add_the_handlers(document.getElementsByTagName("button"));
var walk_the_DOM=function walk(node, func){
        func(node);
        node=node.firstChild;
        while(node){
                walk(node, func);
                node=node.nextSibling;
        }
};

var getElementsByAttribute=function(attr, value){
        var  results=[];
        walk_the_DOM(document.body, function(node){
                var actual=node.nodeType===1 && node.getAttribute(attr);
                if(typeof actual==='string' &&
                  (actual===value || typeof value !== 'string')){
                        results.push(node);
                }
        });
        return results;
};
// var objs=getElementsByAttribute("type", "button");
// for(var i=0; i<objs.length; i++){
//         document.writeln(objs[i]);
// }

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

// eventuality(myCoolCat);
// myCoolCat.health = 100;
// myCoolCat.on('dead', function (event) {
//     var date = event.date;
//     var killer = event.killer;
//     console.log("%s killed me at %s", killer, date);
// });
// myCoolCat.on('hurt', function () {
//     this.health -= 50;
//     if (this.health <= 0) {
//         var event = {
//             type: 'dead',
//             killer: 'stone from sky',
//             date: new Date()
//         };
//         this.fire(event);
//     } else {
//         console.log('nothing, who is the strong guy.');
//     }
// });

// myCoolCat.fire("hurt"); //nothing, who is the strong guy.
// myCoolCat.fire("hurt");

//判断是数组还是对象
var is_array=function(value){
    return Object.prototype.toString.apply(value)==='[object Array]';
};
Array.method('reduce', function(f,value){
    var i;
    for(i=0; i<this.length; i+=1){
        value=f(this[i], value);
    }
    return value;
});
// var add=function(a, b){
//     return a+b;
// };
// var mult=function(a, b){
//     return a*b;
// };

Array.dim=function (dimension, initial){
    var a=[], i;
    for(i=0; i<dimension;i+=1){
        a[i] = initial;
        return a;
    }
};
// var myArray=Array.dim(10, 0);
Array.matrix=function(m, n, initial){
    var a, i, j, mat=[];
    for(i=0;i<m; i+=1){
        a=[];
        for(j=0; j<n; j+=1) {
            a[j]=initial;
        }
        mat[i] = a;
    }
    return mat;
};

//构建一个单位矩阵的方法
Array.identity=function(n){
    var i, mat=Array.matrix(n, n, 0);
    for(i=0; i<n; i+=1) {
        mat[i][i]=1;
    }
    return mat;
};

// var myMatrix=Array.identity(4);
// console.log(myMatrix);
