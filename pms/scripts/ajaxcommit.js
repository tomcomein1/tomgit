/*ajax相关函处理*/
function addLoadEvent(func) {
  var oldonload = window.onload;
  if (typeof window.onload != 'function') {
    window.onload = func;
  } else {
    window.onload = function() {
      oldonload();
      func();
    }
  }
}

function insertAfter(newElement, targetElement) {
  var parent = targetElement.parentNode;
  if (parent.lastChild == targetElement) {
    parent.appendChild(newElement);
  } else {
    parent.insertBefore(newElement, targetElement.nextSibling);
  }
}
/**/
function addClass(element, value) {
  if (!element.className) {
    element.className = value;
  } else {
    newClassName = element.className;
    newClassName += " ";
    newClassName += value;
    element.className = newClassName;
  }
}

/**统一封装ajax调用方法***/
function ajaxCommit(url, method, data, tagid) {
  var xmlHttp;
  xmlHttp = GetXmlHttpObject();
  if (xmlHttp == null) {
    alert("Browser does not support HTTP Request");
    return false;
  }
  //alert(url + "," + method + "," + data + "," + tagid);
  console.log(url + ":"+method+","+data+"," + tagid);
  if (method === "GET") {
    xmlHttp.open("GET", url + "?" + data + "&sid=" + Math.random());
    xmlHttp.send();
  } else if (method === "POST") {
    xmlHttp.open("POST", url, true);
    xmlHttp.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
    xmlHttp.send(data);
  }

  xmlHttp.onreadystatechange = function() {
    if (xmlHttp.readyState === 4) {
      if (xmlHttp.status === 200) {
        document.getElementById(tagid).innerHTML = xmlHttp.responseText;
      }
    }
  }
}

function GetXmlHttpObject() {
  var xmlHttp = null;
  try {
    // Firefox, Opera 8.0+, Safari
    xmlHttp = new XMLHttpRequest();
  } catch(e) {
    //Internet Explorer
    try {
      xmlHttp = new ActiveXObject("Msxml2.XMLHTTP");
    } catch(e) {
      xmlHttp = new ActiveXObject("Microsoft.XMLHTTP");
    }
  }
  return xmlHttp;
}

//表单是否有值检查
function isFiled(field) {
  if (field.value.length < 1 || field.value == field.defaultValue) {
    return false;
  } else {
    return true;
  }
}

/*非空检查*/
function isNull(field) {
  if (field.value.length < 1) {
    return false;
  } else {
    return true;
  }
}

function isEmail(field) {
  if (field.value.indexOf("@") == -1 || field.value.indexOf(".") == -1) {
    return false
  } else {
    return true;
  }
}

function isNumber(field) {
  if (isNaN(field.value)) {　
    return false;
  } else {
    return true;
  }
}

function isAmt(filed) {
  var parse_number = /^-?\d+(?:\.\d*)?(?:e[+\-]?\d+)?$/i;
  test = function(num) {
    return parse_number.test(num);
  }
  return test(filed.value);
}

function isContains(str, substr) {
  return str.indexOf(substr) >= 0;
}

function checkInput() {
  var rows = document.getElementsByTagName("input");
  //取Input名字与值
  for (var i = 0; i < rows.length; i++) {
    if (rows[i].className.indexOf("required") != -1) {
      // if(!isFiled(rows[i])) {
        if (!isNull(rows[i])) {
          alert("Please fill in the " + rows[i].name + " field.");
          return false;
        }
      }
      if (rows[i].className.indexOf("phone") != -1) {
        if (!isNumber(rows[i])) {
          alert("The " + rows[i].name + " field must be a valid phone number.");
          return false;
        }
      }
      if (rows[i].className.indexOf("amt") != -1) {
        if (!isAmt(rows[i])) {
          alert("The " + rows[i].name + " field must be a valid phone number.");
          return false;
        }
      }
    }
    return true;
  }

//页面提交，收集相应的元素拼接成字符串进行提交
function pubSubmit(url, id, method, tagid) {
  if (!document.getElementsByTagName) return false
    if (!checkInput()) return false;
  data = typeTextSelect();

  data += "id=" + id;
  //alert(url + ":" + method + ", " + tagid);
  //alert(data);
  ajaxCommit(url, method, data, tagid);
  return true;
}

/*根据时间刷新当前页面*/
function flushPage(second) {　　
  setTimeout("location.reload(true);", second * 1000);
}

/*动态创建表单的json定义*/
var getTableInsertJson=function getTableInsertJson(table){
  if(table==="booking") {
    var jsonObj = [
    {"enname": "客户编号", "name": "custid", "type": "text", "value": "", "len": "8", "read": true, "check": false },
    {"enname": "宝宝名称", "name": "baby_name", "type": "text", "value": "", "len": "60", "read": true, "check": false },
    {"enname": "预约拍摄日期", "name": "book_date", "type": "date", "value": "", "len": "10", "read": false, "check": true, "cl":"required" },
    {"enname": "预约拍摄时间", "name": "book_time", "type": "time", "value": "", "len": "30", "read": false, "check": false },
    {"enname": "预约套系", "name": "suit", "type": "select", "value": "", "len": "20", "read": false, "check": false },
    {"enname": "渠道类型", "name": "chnl", "type": "select", "value": "", "len": "10", "read": false, "check": false },
    {"enname": "预约拍摄方式", "name": "method", "type": "select", "value": "", "len": "10", "read": false, "check": false },
    {"enname": "订单编号", "name": "order_no", "type": "text", "value": "", "len": "20", "read": false, "check": false },
    {"enname": "备注", "name": "book_memo", "type": "textarea", "value": "", "len": "120", "read": false, "check": false }
    ];
  } else if(table==="photography") {
    var jsonObj = [
    {"enname": "客户编号", "name": "custid", "type": "text", "value": "",  "len":"8","read": true, "check":false },
    {"enname": "预约编号", "name": "bookid", "type": "text", "value": "",  "len":"8","read": true, "check":false },
    {"enname": "订单编号", "name": "order_no", "type": "text", "value": "", "len": "20", "read": false, "check": false },
    {"enname": "宝宝姓名", "name": "cust_name", "type": "text", "value": "",  "len":"60","read": true, "check":false },
    {"enname": "拍摄日期", "name": "photo_date", "type": "date", "value": "", "len":"10", "read": false, "check":true, "cl":"required" },
    {"enname": "摄影师", "name": "photo_man", "type": "text", "value": "龚强", "len":"10", "read": false, "check":true, "cl":"required" },
    {"enname": "拍摄备注", "name": "photo_memo", "type": "textarea", "value": "", "len":"120", "read": false, "check":false }
    ];
  } else {
    alert("not this table[" + table + "] obj.");
    var jsonObj=[];
  }
  return jsonObj;
};

var tableInsertClick=function tableInsertClick(table, col, id){
  if (!document.getElementsByTagName) return false;
  var rows = document.getElementsByTagName("tr");

  var jobj = getTableInsertJson(table);

  var suit="", chnl="", method="";
  var contents = "";
  var value = "";
  var i=0, j=0;
  for (i = 0; i < rows.length; i+=1) {
    if (rows[i].cells[0].innerHTML != id) {
      continue;
    }
    for (j=0; j < jobj.length; j+=1) {

      if(j < col) {
         value=rows[i].cells[j].innerHTML;
      } else {
         value=jobj[j].value;
      }
//      console.log("value="+ value  + "===j=" + j  + "col=", col + "id=" + id);
      if(jobj[j].name==="suit"){
        suit=value;
      } else if(jobj[j].name==="chnl"){
        chnl=value;
      } else if(jobj[j].name==="method"){
        method=value;
      }
      contents += createElementByJson(jobj, jobj[j].enname, value, j);
    }
    contents += "<button id='insert_" + table + "' >提交</button>";
//    console.log(contents);
    break;
  }

  document.getElementById("dynamic_content").innerHTML = contents;
  document.getElementById("ajaxret").innerHTML = "";
  if(document.getElementById("suitret")) {
    getServerSelect("oper=suit&value="+suit, "suitret");
  }
  if(document.getElementById("chnlret")) {
    getServerSelect("oper=chnl&value="+chnl, "chnlret");
  }
  if(document.getElementById("methodret")) {
    getServerSelect("oper=method&name=method&value="+method, "methodret");
  }
  add_the_handlers(document.getElementsByTagName("button"));
  return true;
};

/**动态表单生成json定义*/
var getTableJson=function getTableJson(table){
  if(table==="channel"){
    var jsonObj=[
    {"name": "chnlid", "type": "text", "value": "", "len": "10", "read": true, "check": true},
    {"name": "chnl",   "type": "text", "value": "", "len": "10", "read": false,"check": true},
    {"name": "chnl_name","type": "text","value": "","len": "30", "read": false,"check":false}
    ];
  } else if(table==="suit"){
    var jsonObj=[
    {"name": "suitid", "type": "text", "value": "", "len": "10", "read": true, "check": true },
    {"name": "suit", "type": "text", "value": "", "len": "10", "read": false, "check": true },
    {"name": "suit_name", "type": "text", "value": "", "len": "30", "read": false,"check":false },
    {"name": "suit_amt", "type": "text", "value": "", "len": "12", "read": false,"check":true, "cl":"amt" },
    {"name": "suit_cont", "type": "textarea", "value": "", "len": "255", "read": false,"check":false}
    ];
  }else if(table==="custom"){
    var jsonObj = [
    {"name": "custid", "type": "text", "value": "", "len": "3", "read": true },
    {"name": "baby_name", "type": "text", "value": "", "len": "60", "read": false, "check": false },
    {"name": "baby_birth", "type": "date", "value": "", "len": "10", "read": false, "check": false },
    {"name": "nick_name", "type": "text", "value": "", "len": "20", "read": false, "check": false },
    {"name": "baby_sex", "type": "radio", "value": "",  "listval":["1","男","0","女"], "read": false, "check": false },
    {"name": "cust_phone", "type": "text", "value": "", "len": "30", "read": false, "check": true, "cl":"phone" },
    {"name": "cust_name", "type": "text", "value": "", "len": "60", "read": false, "check": false },
    {"name": "qq", "type": "text", "value": "", "len": "20", "read": false, "check": false },
    {"name": "chnl", "type": "select", "value": "", "len": "10", "read": false, "check": false },
    {"name": "addr", "type": "text", "value": "", "len": "128", "read": false, "check": false },
    {"name": "mb_no", "type": "text", "value": "", "len": "22", "read": false, "check": false },
    {"name": "memo", "type": "textarea", "value": "", "len": "120", "read": false, "check": false },
    {"name": "reg_date", "type": "text", "value": "", "len": "10", "read": true, "check": false },
    {"name": "datetime", "type": "text", "value": "", "len": "25", "read": true, "check": false }
    ];
  } else if(table==="goods"){
    var jsonObj=[
    {"name": "goodsid", "type": "text", "value": "", "len": "5", "read": true, "check": true },
    {"name": "goods_name", "type": "text", "value": "", "len": "30", "read": false, "check": true, "cl":"required" },
    {"name": "goods_number", "type": "text", "value": "", "len": "5", "read": false,"check":true, "cl":"phone" },
    {"name": "goods_stand", "type": "text", "value": "", "len": "20", "read": false,"check":false },
    {"name": "goods_memo", "type": "textarea", "value": "", "len": "60", "read": false,"check":false }
    ];
  }else if(table==="order"){
    var jsonObj=[
    {"name": "orderid", "type": "text", "value": "", "len": "8", "read": true, "check": false },
    {"name": "order_no", "type": "text", "value": "", "len": "20", "read": true, "check": false },
    {"name": "order_date", "type": "date", "value": "", "len": "10", "read": false,"check":false },
    {"name": "suit", "type": "select", "value": "", "len": "20", "read": true, "check": false },
    {"name": "first_date", "type": "date", "value": "", "len": "10", "read": false,"check":false },
    {"name": "first_man", "type": "text", "value": "", "len": "10", "read": false,"check":false },
    {"name": "cp_time", "type": "datetime-local", "value": "", "len": "22", "read": false,"check":false },
    {"name": "cp_method", "type": "select", "value": "", "len": "8", "read": false, "check": false },
    {"name": "cp_cont", "type": "textarea", "value": "", "len": "60", "read": false,"check":false },
    {"name": "second_date", "type": "date", "value": "", "len": "10", "read": false,"check":false },
    {"name": "second_man", "text": "date", "value": "", "len": "10", "read": false,"check":false },
    {"name": "pm_date", "type": "date", "value": "", "len": "10", "read": false,"check":false },
    {"name": "ret_method", "type": "select", "value": "", "len": "8", "read": false,"check":false },
    {"name": "ret_circs", "type": "textarea", "value": "", "len": "60", "read": false,"check":false },
    {"name": "get_photo", "type": "datetime-local", "value": "", "len": "22", "read": false,"check":false },
    {"name": "get_method", "type": "select", "value": "", "len": "8", "read": false,"check":false },
    {"name": "get_memo", "type": "textarea", "value": "", "len": "60", "read": false,"check":false },
    {"name": "order_stat", "type": "text", "value": "", "len": "1", "read": false,"check":false },
    {"name": "order_memo", "type": "textarea", "value": "", "len": "255", "read": false,"check":false },
    {"name": "datetime", "type": "text", "value": "", "len": "22", "read": true,"check":false }
    ];
  }else if(table==="acctreg"){
    var jsonObj = [
    {"name": "regid", "type": "text", "value": "", "len": "5", "read": true, "check": false },
    {"name": "acct_date", "type": "date", "value": "", "len": "10", "read": false, "check": true, "cl": "required" },
    {"name": "dr_cr", "type": "radio", "value": "", "len": "2",  "listval":["1","支出","2","收入"], "read": false, "check": false},
    {"name": "acct_amt", "type": "text", "value": "", "len": "17", "read": false, "check": true, "cl": "require amt" },
    {"name": "pay_mode", "type": "text", "value": "", "len": "10", "read": false, "check": false },
    {"name": "pay_man", "type": "text", "value": "", "len": "10", "read": false, "check": false },
    {"name": "reg_man", "type": "text", "value": "", "len": "10", "read": false, "check": false },
    {"name": "user_phone", "type": "text", "value": "", "len": "15", "read": false, "check": true, "cl": "required phone" },
    {"name": "reg_memo", "type": "textarea", "value": "", "len": "120", "read": false, "check": false },
    {"name": "reg_date", "type": "date", "value": "", "len": "10", "read": false, "check": false },
    {"name": "is_valid", "type": "text", "value": "", "len": "1", "read": true, "check": false },
    {"name": "datetime", "type": "text", "value": "", "len": "22", "read": true, "check": false }
    ];
  }else if(table==="booking"){
    var jsonObj = [
    {"name": "bookid", "type": "text", "value": "", "len": "10", "read": true },
    {"name": "book_date", "type": "date", "value": "", "len": "10", "read": false, "check": true },
    {"name": "book_time", "type": "time", "value": "", "len": "6", "read": false, "check": false },
    {"name": "suit", "type": "select", "value": "", "len": "20", "read": false, "check": false },
    {"name": "chnl", "type": "select", "value": "", "len": "10", "read": false, "check": false },
    {"name": "method", "type": "select", "value": "", "len": "10", "read": false, "check": false },
    {"name": "custid", "type": "text", "value": "", "len": "10", "read": true, "check": false },
    {"name": "order_no", "type": "text", "value": "", "len": "120", "read": false, "check": false },
    {"name": "book_memo", "type": "textarea", "value": "", "len": "120", "read": false, "check": false },
    {"name": "datetime", "type": "text", "value": "", "len": "22", "read": true, "check": false },
    {"name": "baby_name", "type": "text", "value": "", "len": "60", "read": true, "check": false },
    {"name": "cust_phone", "type": "text", "value": "", "len": "30", "read": true, "check": false }
    ];
  } else if(table==="photography") {
    var jsonObj = [
    {"name": "photoid", "type": "text", "value": "", "len": "8", "read": true, "check": false },
    {"name": "photo_date", "type": "date", "value": "", "len": "10", "read": false, "check": false },
    {"name": "photo_man", "type": "text", "value": "", "len": "10", "read": false, "check": false },
    {"name": "photo_memo", "type": "textarea", "value": "", "len": "120", "read": false, "check": false },
    {"name": "bookid", "type": "text", "value": "", "len": "8", "read": true, "check": false },
    {"name": "custid", "type": "text", "value": "", "len": "8", "read": true, "check": false },
    {"name": "order_no", "type": "text", "value": "", "len": "20", "read": false, "check": false },
    {"name": "datetime", "type": "text", "value": "", "len": "22", "read": true, "check": false },
    {"name": "baby_name", "type": "text", "value": "", "len": "60", "read": true, "check": false },
    {"name": "cust_name", "type": "text", "value": "", "len": "30", "read": true, "check": false },
    {"name": "cust_phone", "type": "text", "value": "", "len": "30", "read": true, "check": false }
    ];
  } else {
    alert("not this table[" + table + "] obj.");
    var jsonObj=[];
  }
  return jsonObj;
};

var tablePubClick=function tablePubClick(table, col, id){
  if (!document.getElementsByTagName) return false;
  var rows = document.getElementsByTagName("tr");

  var jobj = getTableJson(table);

  var suit="", chnl="", method="", value="";
  var cp_mthod="", ret_method="", get_method="";
  var contents = "";
  for (var i = 0; i < rows.length; i++) {
    if (rows[i].cells[0].innerHTML != id) {
      continue;
    }
    for (var j = 0; j < col; j++) {
      value=rows[i].cells[j].innerHTML;
      if(jobj[j].name==="suit" && jobj[j].type==="select"){
        suit=value;
      } else if(jobj[j].name==="chnl" && jobj[j].type==="select"){
        chnl=value;
      } else if(jobj[j].name==="method" && jobj[j].type==="select"){
        method=value;
      } else if(jobj[j].name==="cp_method" && jobj[j].type==="select"){
        cp_method=value;
      } else if(jobj[j].name==="ret_method" && jobj[j].type==="select"){
        ret_method=value;
      } else if(jobj[j].name==="get_method" && jobj[j].type==="select"){
        ret_method=value;
      }
      contents += createElementByJson(jobj, rows[0].cells[j].innerHTML, value, j);
    }
    contents += "<button id='update_" + table + "' >提交</button>";
//    console.log(contents);
    break;
  }

  document.getElementById("dynamic_content").innerHTML = contents;
  document.getElementById("ajaxret").innerHTML = "";
  if(document.getElementById("suitret") ) {
    getServerSelect("oper=suit&value="+suit, "suitret");
  }
  if(document.getElementById("chnlret") ) {
    getServerSelect("oper=chnl&value="+chnl, "chnlret");
  }

  if(document.getElementById("methodret") ) {
    getServerSelect("oper=method&name=method&value="+method, "methodret");
  }

  if(document.getElementById("cp_methodret") ) {
    getServerSelect("oper=method&name=cp_method&value="+cp_method, "cp_methodret");
  }
  if(document.getElementById("ret_methodret") ) {
    getServerSelect("oper=method&name=ret_method&value="+ret_method, "ret_methodret");
  }
  if(document.getElementById("get_methodret") ) {
    getServerSelect("oper=method&name=get_method&value="+get_method, "get_methodret");
  }

  add_the_handlers(document.getElementsByTagName("button"));
  return true;
};

function createElementByJson(jobj, thvalue, tdvalue, col)
{
    //此处生成动态网页
    var result = "";
    var rstr = "";
    result += "<p><label class='l_width'>" + thvalue + ":</label>\n";
    //控制越界
  if(col < jobj.length) {
    if (jobj[col].type === "radio") {
        var listval = jobj[col].listval ;
        var num = listval.length  / 2 ;
        for ( var i = 0 , j = 0 ; i < num ; i++ , j = j + 2 )
        {
            rstr += "<label><input type='" + jobj[col].type + "' id='" + jobj[col].name + listval[j] + "' name='" + jobj[col].name + "' value='" + listval[j] + "' ";
            //console.log("DEBUG==>" + listval[j] + "====" + tdvalue);
            if ( listval[j] == tdvalue ) {
                //默认选择上
                rstr += " checked='checked' ";
            }
            if (jobj[col].read == true) {
                result += " class='color_ccc' readonly='true' ";
            }
            rstr += "></input>" + listval[j + 1] + "</label>"
        }
        rstr += "</p>";
        result += rstr;
    } else if ( jobj[col].type == "list" ) {
      var listval = jobj[col].listval;
      var num = listval.length  / 2;
      var rstr = "<select name='" + jobj[col].name + "'>";
      for (var i = 0 , j = 0; i < num ; i++, j = j + 2 )
      {
        rstr += "<option ";
        if ( tdvalue == listval[j] ) {
          rstr += "selected='selected' ";
        }
        rstr += "value='" + listval[j] + "'>" + listval[j + 1] + "</option>";
      }
      rstr += "</select>";
      result += rstr;
    } else if (jobj[col].name == "chnl" && jobj[col].type=="select") {
      result += "<span id='chnlret'></span>";
      result += "</p>";
    } else if (jobj[col].name == "suit" && jobj[col].type=="select") {
      result += "<span id='suitret'></span>";
      result += "</p>";
    } else if (jobj[col].name == "method" && jobj[col].type=="select") {
      result += "<span id='methodret'></span>";
      result += "</p>";
    } else if (jobj[col].name == "cp_method" && jobj[col].type=="select") {
      result += "<span id='cp_methodret'></span>";
      result += "</p>";
    } else if (jobj[col].name == "ret_method" && jobj[col].type=="select") {
      result += "<span id='ret_methodret'></span>";
      result += "</p>";
    } else if (jobj[col].name == "get_method" && jobj[col].type=="select") {
      result += "<span id='get_methodret'></span>";
      result += "</p>";
    }else if(jobj[col].type=="textarea"){
      result += "<textarea ";
      result += "name='" + jobj[col].name + "' ";
      result += "maxlength='" + jobj[col].len + "'>";
      result += tdvalue+"</textarea></p>";
    }else {
        result += "<input type='" + jobj[col].type + "' ";
        result += "name='" + jobj[col].name + "' ";
        result += "maxlength='" + jobj[col].len + "' ";
        result += "value='" + tdvalue + "' ";
        if (jobj[col].read === true) {
            result += " class='color_ccc' readonly='true' ";
        }
        else if (jobj[col].check === true) {
            result += " class='" + jobj[col].cl + "' ";
        }
        result += "></input></p>\n";
    }
  }
    return result;
}

/*获取后端参数类数据*/
function getServerSelect(data, tagid) {
            ajaxCommit("action/select.php", "GET", data, tagid);
          }

function delPubfunction(data, id) {
  // var sdata = data + id + "&sid=" + Math.random();
  var sdata=data + "&sid=" + Math.random();
  var conf=confirm("你确定要删除该记录吗?");
  if(conf===true){
    ajaxCommit("action/pubinterface.php", "POST", sdata, "ajaxret");
  }
}

function getWeekSelect(name) {
  var str = "<select name='" + name + "'>";

  str += "<option selected='selected' value='1'>星期一</option>";
  str += "<option value='2'>星期二</option>";
  str += "<option value='3'>星期三</option>";
  str += "<option value='4'>星期四</option>";
  str += "<option value='5'>星期五</option>";
  str += "<option value='6'>星期六</option>";
  str += "<option value='0'>星期日</option>";
  str += "</select>";
  return str;
}

var add_the_handlers = function(nodes) {
  var click = function(element) {
    return function(e) {
      //alert(element.id);
      if(isContains(element.id, "sel")) {
        if(document.getElementById("dynamic_content")){
          document.getElementById("dynamic_content").innerHTML="";
        }
      }

      if(isContains(element.id, "update") || isContains(element.id, "insert")) {
          var conf=confirm("你确定要提交吗?");
          if(conf===false){
              return false;
          }
	  if(document.getElementById("ajaxret")) {
              document.getElementById("ajaxret").innerHTML="欢迎光临泡泡糖儿童摄影";
	  }
      }
      var ret=pubSubmit("action/pubinterface.php", element.id, "POST", "ajaxret");
    }
  };
  var i;
  for (i = 0; i < nodes.length; i += 1) {
    // if (isContains(nodes[i].id, "del")
    //   || nodes[i].id===""){
    //   continue;
    // }
    nodes[i].onclick = click(nodes[i]);
  }
};
add_the_handlers(document.getElementsByTagName("button"));

//遍列结点
var walk_the_DOM = function walk(node, func) {
  func(node);
  node = node.firstChild;
  while (node) {
    walk(node, func);
    node = node.nextSibling;
  }
};

var getElementsByAttribute = function(attr, value) {
  var results = [];
  walk_the_DOM(document.body,
    function(node) {
      var actual = node.nodeType === 1 && node.getAttribute(attr);
      if (typeof actual === 'string' && (actual === value || typeof value !== 'string')) {
        results.push(node);
      }
    });
  return results;
};

function typeTextSelect() {
  var rows = document.getElementsByTagName("input");
  var str = "";
  /*取Input名字与值**/
  for (var i = 0; i < rows.length; i++) {
    if (rows[i].type == "radio") {
      if (!rows[i].checked) continue;
    }

    //替换文本框中的单引号为双引号
    var value=rows[i].value.replace(/'/g, '"');
    //str += rows[i].name + "=" + rows[i].value;
    str += rows[i].name + "=" + value;
    str += "&";
  }
  /**取单选框名字与值*/
  var select = document.getElementsByTagName("select");
  for (var i = 0; i < select.length; i++) {
    index = select[i].selectedIndex;
    //alert(select[i].name+":"+index);
    if(index!=-1){
      str += select[i].name + "=" + select[i].options[index].value + "&";
    }
  }

  var textarea=document.getElementsByTagName("textarea");
  for (var i = 0; i < textarea.length; i++) {
    //替换文本框中的单引号为双引号
    var value=textarea[i].value.replace(/'/g, '"');
    //str += rows[i].name + "=" + rows[i].value;
    str += textarea[i].name + "=" + value;
    str += "&";
  }

  return str;
}
//改变节点背景色
var fade = function(node) {
  var level = 1;
  var step = function() {
    var hex = level.toString(16);
    node.style.backgroundColor = '#FFFF' + hex + hex;
    if (level < 15) {
      level += 1;
      setTimeout(step, 100);
    }
  };
  setTimeout(step, 100);
};
//fade(document.body);
var getSelectInfo=function(element, str, id){

  var onchange=function(str, id){
    getServerSelect(str, id);
  };

  element.onchange=onchange(str, id);
};

/*取得后台列表框事件*/
if(document.getElementById("getchnlinfo")){
  getSelectInfo(document.getElementById("getchnlinfo"), "oper=chnl&value=''", "chnlret");
}
if(document.getElementById("getsuitinfo")){
  getSelectInfo(document.getElementById("getsuitinfo"), "oper=suit&value=''", "suitret");
}

if(document.getElementById("getcpmethod")){
  getSelectInfo(document.getElementById("getcpmethod"), "oper=method&name=cp_method&value=''", "cp_methodret");
}
if(document.getElementById("getretmethod")){
  getSelectInfo(document.getElementById("getretmethod"), "oper=method&name=ret_method&value=''", "ret_methodret");
}
if(document.getElementById("getgetmethod")){
  getSelectInfo(document.getElementById("getgetmethod"), "oper=method&name=get_method&value=''", "get_methodret");
}
if(document.getElementById("todaywarn")){
  getSelectInfo(document.getElementById("todaywarn"), "oper=todaywarn&value=''", "dynamic_content");
}

if(document.getElementById("navbar-menu")){
  getSelectInfo(document.getElementById("navbar-menu"), "oper=get_username&value=''", "username");
}


//第一个文本框取得焦点事件 并且文本框取得焦点后清空
var inputFocus=function() {
  var inputs=document.getElementsByTagName("input");
  for(var i=0; i<inputs.length; i+=1) {
     inputs[0].focus();
     inputs[i].onfocus=function(){
        if(this.type=="text" && this.value!=this.defaultValue ) {
	   this.value=""; 
	}
     }
     inputs.onblur=function(){
       if(this.type=="text" && this.value==""){
          this.value=this.defaultValue;
        }
    }

  }
}
inputFocus();

