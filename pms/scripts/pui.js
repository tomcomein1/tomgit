/*ǰ��Ԫ�ط�װ(javascript)
* author: TOM
* version: 1.0
* date: 20170520
*/

var MYAPP={};
MYAPP.stooge={
	"first-name":"tom",
	"last-name":"tracy"
};

MYAPP.flight={
	airline: "Oceanic",
	number:815,
	departure: {
		IATA:"SYD",
		time: "2017-05-20 11:22",
		city: "Chengdu"
	},
	arrival: {
		IATA:"LAX",
		time: "2017-05-20 11:22",
		city: "Shanghi"
	}
};

var myObject=(function(){
	var value=0;

	return {
		increment:function(inc) {
			value += typeof inc==='number'?inc:1;
		},
		getValue:function(){
			return value;
		}
	};
}());

myObject.double=function(){
	var that=this;
	var helper=function(){
		//that.value=add(that.value, that.value);
		document.writeln("helper");
	}
	helper();
}

myObject.increment();
document.writeln(myObject.value);
myObject.increment(2);
document.writeln(myObject.value);

myObject.double();
document.writeln(myObject.value);

var Quo=function(string){
	this.status=string;
};
Quo.prototype.get_status=function(){
	return this.status;
};
var array=[3,4];
//var sum=add.apply(null,array);
var statusObject={
	status:'A-OK'
};
//�˴�����Ҫ��new����������ʹ����apply
var status=Quo.prototype.get_status.apply(statusObject);
//��̬��������
var sum=function(){
	var i,sum=0;
	for(i=0; i<arguments.length; i++){
		sum += arguments[i];
	}
	return sum;
};
document.writeln(sum(4,9,23,44,33));
//�쳣
var add=function(a,b){
	if(typeof a!== 'number' || typeof b!== 'number'){
		throw{
			name:'TypeError',
			message: 'add needs numbers'
		};
	}
	return a+b;
};
var try_it=function(){
	try{
		add("seven");
	}catch(e) {
		document.writeln(e.name + ': ' + e.message);
	}
};
try_it();

Function.prototype.method=function(name, func){
	if(!this.prototype[name]){
		this.prototype[name]=func;
	}
	return this;
};

Number.method('integer', function(){
	return Math[this<0 ? 'ceil' : 'floor'](this);
});

document.writeln((-10/3).integer()); //-3
//�Ƴ��ַ�����β�հ׵ķ���
String.method('trim', function(){
	return this.replace(/^\s+|\s+$/g,'');
});

document.writeln('"' + "    neat   ".trim() + '"');

var hanoi=function(disc, src, aux, dst){
	if(disc>0) {
		hanoi(disc-1, src, dst, aux);
		document.writeln('Move disc ' + disc +
			' from ' + src + ' to ' + dst);
			hanoi(disc-1, aux, src, dst);
	}
};
hanoi(3, 'Src', 'Aux', 'Dst');

//���н��
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

var objs=getElementsByAttribute("type", "button");
for(var i=0; i<objs.length; i++){
	document.writeln(objs[i]);
}

//β�ݹ�
var factorial=function factorial(i, a){
	a=a||1;
	if(i<2) {
		return a;
	}
	return factorial(i-1, a*i);
};

document.writeln(factorial(4));

//����һ��quo ״ֵ̬Ϊ˽��
var quo=function(status){
	return {
		get_status:function(){
			return status;
		}
	};
};
var myQuo=quo("amazed");
document.writeln(myQuo.get_status());

//�ı�ڵ㱳��ɫ
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
fade(document.body);

var add_the_handlers=function(nodes){
	var helper=function(element){
		return function(e) {
			var str='';
			if(element.id !== "")
			  str += 'id: ' + element.id;
			str += ' value: ' + element.innerHTML;
			alert(str);
		};
	}

	var i;
	for(i=0; i<nodes.length; i+=1){
		nodes[i].onclick=helper(nodes[i]);
	}
};

add_the_handlers(document.getElementsByTagName("h2"));
//�ص�
/**
request=prepare_the_request();
send_request_asynchronously(request, function(response){
	display(response);
});
**/
String.method('deentityify', function(){
	var entity={
		quot: '"',
		lt: '<',
		gt: '>'
	};
	//����deentityify����
	return function(){
		//����'&'��ͷ��';'�������ַ��������ַ�ʵ���滻Ϊӳ����е�ֵ
		return this.replace(/&([^&;]+);/g,
			function(a, b){
				var r=entity[b];
				return typeof r==='string' ? r : a;
			}
		);
	};
}());
document.writeln('&lt;&quot;&gt;'.deentityify( ));

//ģ��ģʽ����һ�������������кŵĶ���
var serial_maker=function(){
	//����һ����������Ψһ�ַ����Ķ���
	//Ψһ�ַ�������������ɣ� ǰ׺+���кš�
	//�ö������һ������ǰ׺�ķ�����һ���������кŵķ���
	//��һ������Ψһ�ַ�����gensym����
	var prefix='';
	var seq=0;
	return {
		set_prefix: function(p){
			prefix=String(p);
		},
		set_seq:function(s){
			seq=s;
		},
		gensym: function() {
			var result=prefix+seq;
			seq+=1;
			return result;
		}
	};
};
var seqer=serial_maker();
seqer.set_prefix('Q');
seqer.set_seq(1000);
var unique=seqer.gensym();
document.writeln(unique);
unique=seqer.gensym();
document.writeln(unique);
//����

Function.method('curry', function(){
	var slice=Array.prototype.slice,
		args=slice.apply(arguments),
		that = this;
	return function(){
		return that.apply(null, args.concat(arguments));
	};
});
//����,�����ǵݹ飬���ô����϶�
//var fibonacci=function(n){
//	return n<2 ? n : fibonacci(n-1) + fibonacci(n-2);
//};

//���䣬�հ�ʵ��
var memoizer=function(memo, formula){
	var recur=function(n){
		var result=memo[n];
		if(typeof result !== 'number') {
			result=formula(recur, n);
			memo[n]=result;
		}
	};
	return recur;
};

var fibonacci=memoizer([0,1], function(recur, n){
	return recur(n-1)+recur(n-2);
});

var factorial=memoizer([1,1], function(recur, n){
	return n*recur(n-1);
});

//for(var i=0; i<10; i+=1){
//	document.writeln('//'+i+':'+factorial(i));
//}
//����new ��������
Function.method('new', function(){
	var that=Object.create(this.prototype);
	var other=this.call(that, arguments);
	return (typeof other==='object' && other)||that;
});
//�̳�
var Mammal=function(name){
	this.name=name;
};
Mammal.prototype.get_name=function() {
	return this.name;
};
Mammal.prototype.says=function(){
	return this.saying || '';
};
var myMammal=new Mammal('Herb the Mammal');
var name=myMammal.get_name();
var Cat=function(name){
	this.name=name;
	this.saying='newow';
};
//�滻Cat.prototypeΪһ����Mammalʵ��
Cat.prototype=new Mammal();
//������ԭ�Ͷ�������purr��get_name����
/*
Cat.prototype.purr=function(n){
	var i,s='';
	for(i=0; i<n; i+=1){
		if(s){
			s += '-';
		}
		s += 'mi';
	}
	return s;
};
Cat.prototype.get_name=function(){
	return this.says() + ' ' + this.name + ' ' + this.says();
};
var myCat=new Cat('Henrietta');
var says=myCat.says();
var purr=myCat.purr(5);
var name=myCat.get_name();
document.writeln(says+','+purr+','+name);
**/
//α��
Function.method('inherits', function(Parent){
	this.prototype=new Parent();
	return this;
});

var Cat=function(name){
	this.name=name;
	this.saying='meow';
}
.inherits(Mammal)
.method('purr', function(n){
var i,s='';
	for(i=0; i<n; i+=1){
		if(s){
			s += '-';
		}
		s += 'mi';
	}
	return s;
})
.method('get_name', function(){
	return this.says() + ' ' + this.name + ' ' + this.says();
});


//����
var myMammal={
	name:'Herb the Mammal',
	get_name:function(){
		return this.name;
	},
	says:function(){
		return this.saying||'';
	}
};
var myCat=Object.create(myMammal);
myCat.name='Henrietta';
myCat.saying='meow';
myCat.purr=function(n){
	var i,s='';
	for(i=0;i<n;i++){
		if(s) s+='-';
		s+='r';
	}
	return s;
};
myCat.get_name=function(){
	return this.says+' '+this.name + ' '+this.says;
};

var block=function(){
	//��ס��ǰ��������,����һ�������˵�ǰ�����������ж������������
	var oldScope=scope;
	scope=Object.create(scope);
	//������������Ϊ��������advance.
	advance('{');
	//ʹ���µ�������ʱ�н���
	parse(scope);
	//�����һ�������Ϊ��������advance�������������򣬻ָ�ԭ���ϵ�������
	advance('}');
	scope=oldScope;
};
//�������α����
var constructor=function(spec, my){
	var that,other_var,other_var1;
	my=my||{};
	//�ѹ���ı����ͺ�����ӵ�my��

	that=this; //һ���¶���
	//���that����Ȩ����
	return that;
};
//�̳�
var methodical=function(){
};
//that.methodical=methodical;

var mammal=function(spec){
	var that={};

	that.get_name=function(){
		return spec.name;
	};
	that.says=function(){
		return spec.saying||'';
	};
	return that;
};
var myMammal=mammal({name:'Herb'});
var cat=function(spec){
	spec.saying = spec.saying||'meow';
	var that=mammal(spec);
	that.purr=function(n){
		var s="mi";
		return s;;
	};
	that.get_name=function(){
		return that.says() + ' ' + spec.name;
	};
	return that;
};
var myCat=cat({name: 'Henrietta'});

//���ø���ķ���
Object.method('superior', function(name){
	var that=this,
		method=that[name];
	return function(){
		return method.apply(that, arguments);
//		return method.call(that, arguments);
	};
});

var coolcat=function(spec){
	var that=cat(spec),
		super_get_name=that.superior('get_name');
		that.get_name=function(n){
			return 'like ' + super_get_name() + ' baby';
		};
		return that;
};
var myCoolCat=coolcat({name: 'Bix'});
var name=myCoolCat.get_name(); //'like meow Bix meow baby'
//ע���¼�����

var eventuality=function(that){
	var registry={};
	//��һ�������ϴ���һ���¼������¼�������һ�������¼����Ƶ��ַ���
	//������һ��ӵ�а������¼����Ƶ�type���ԵĶ���
	//ͨ��'on'����ע����¼����������ƥ���¼����Ƶĺ�����������
	that.fire=function(event){
		var array,
			func,
			handler,
			i,
			type=typeof event==='string' ? event : event.type;
			//�������¼�����һ���¼����������ô�ͱ������ǲ���˳������ִ�С�
			if(registry.hasOwnProperty(type)){
				array=registry[type];
				for(i=0; i<array.length; i++){
					handler=array[i];

					//ÿ������������һ��������һ���ѡ�Ĳ���
					//����÷�����һ���ַ�����ʽ�����֣���ôѰ�ҵ��ú���.
					func=handler.method;
					if(typeof func==='string'){
						func=this[func];
					}
					//����һ����������������Ŀ��������,��ô�������ǹ�ȥ�����򣬴��ݸ��¼�����.
					//apply�ɸĳ�call
					func.apply(this, handler.parameters || [event]);
				}
			}
			return this;
	};
	that.on=function(type, method, parameters){
		//ע��һ���¼�������һ�����������Ŀ���������뵽�������������
		//����������͵��¼��������ڣ��͹���һ��.
		var handler={
			method:method,
			parameters: parameters
		};
		if(registry.hasOwnProperty(type)){
			registry[type].push(handler);
		} else {
			registry[type]=[handler];
		}
		return  this;
	};
	return that;
};
//ö��
myArray=[
	"abcd",123, {"name":"tom"},{"type":true}
];
for(i=0; i<myArray.length; i++){
	document.writeln(myArray[i]);
}

//is_array��������Ͷ���
var is_array=function(value){
	return Object.prototype.toString.apply(value)==='[object Array]';
};
document.writeln(is_array(myArray));
document.writeln(is_array(myCoolCat));

//��������������
Array.method('reduce', function(f, value){
	var i;
	for(i=0; i<this.length; i++){
		value=f(this[i], value);
	}
	return value;
});
var data=[4, 5, 6, 12];
var add=function(a,b){
	return a+b;
};
var mult=function(a,b){
	return a*b;
};
var sum=data.reduce(add, 0);
var product=data.reduce(mult, 1);
data.total=function(){
	return this.reduce(add, 0);
};
total = data.total();
document.writeln(sum+','+product+','+total);

Array.dim=function(dimension, initial){
	var a=[],i;
	for(i=0; i<dimension; i++){
		a[i]=initial;
	}
	return a;
};
var myArray=Array.dim(10,0);
Array.matrix=function(m, n, initial){
	var a,i,j,mat=[];
	for(i=0;i<m;i++){
		a=[];
		for(j=0;j<n;j++){
			a[j]=initial;
		}
		mat[i]=a;
	}
	return mat;
};
//����һ��4X4������
var myMatrix=Array.matrix(4,4,0);
document.writeln(myMatrix[3][3]);
//��������һ����λ����ķ���
Array.identity=function(n){
	var i,mat=Array.matrix(n,n,0);
	for(i=0; i<n; i++){
		mat[i][i]=1;
	}
	return mat;
};
myMatrix=Array.identity(4);
document.writeln(myMatrix[3][3]);

var parse_number= /^-?\d+(?:\.\d*)?(?:e[+\-]?\d+)?$/i;
var test=function(num){
	document.writeln(parse_number.test(num));
};
test('1');
test('number');
test('98.6');
Array.method('pop', function(){
	return this.splice(this.length-1, 1)[0];
});
Array.method('push',function(){
	this.splice.applay(
		this,
		[this.length, 0].
			concat(Array.prototype.slice.apply(arguments)));
	return this.length;
});
Array.method('shift',function(){
	return this.splice(0,1)[0];
});

var n=[4,7,23,15,23,45];
n.sort(function (a,b){
	return a-b;
});
var m=['aa','bb','a',4,8,15,16,23,43];
m.sort(function(a,b){
	if(a===b) {
		return 0;
	}
	if(typeof a=== typeof b){
		return a<b?-1:1;
	}
	return typeof a< typeof b? -1:1;
});
var by=function(name){
	return function(o,p){
		var a,b;
		if(typeof o==='object' && typeof p==='object' && o && p){
			a=o[name];
			b=p[name];
			if(a===b){
				return 0;
			}
			if(typeof a=== typeof b){
				return a<b?-1:1;
			}
			return typeof a<typeof b?-1:1;
		}else{
			throw{
				name:'Error',
				message: 'Expected an object when sorting by ' + name
			};
		}
	};
};
var s=[
{first: 'Joe', last:'Besser'},
{first: 'Moe', last:'Howard'},
{first: 'Joe', last:'DeRita'},
{first: 'Shemp', last:'Howard'},
];
s.sort(by(s.first));
