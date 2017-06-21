var show_inner_html=function(el){
    var elments=document.getElementsByTagName(el);
    var evalue='';

    for (var i=0; i<elments.length; i++){
        // alert(elments[i].value);
        // alert(elments[i].type);
        if(elments[i].type==="text" || elments[i].type==="textarea"){
            evalue=elments[i].value;
            // alert(elvalue);
        } else {
            evalue=elments[i].innerHTML;
        }
        alert(evalue);
        if(evalue===''){
            alert('字段不能为空');
            return false;
        }
        console.log(evalue);
    }
};

var add_the_handlers = function(nodes) {
    var click = function(element) {
        return function(e) {
            show_inner_html('input');
        };
    };

    for (var i = 0; i < nodes.length; i += 1) {
        nodes[i].onclick = click(nodes[i]);
    }
};
add_the_handlers(document.getElementsByTagName("button"));

var page_enable_edit=function(stat){
    document.designMode=stat;
};
// page_enable_edit("off");
