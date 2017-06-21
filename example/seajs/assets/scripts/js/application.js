define(function(require,exports,module){
   var util=require('./util');

   var seajs=document.getElementById('hello-seajs');
   seajs.style.color=util.randomColor();
   window.setInterval(function() {
     seajs.style.color=util.randomColor();
   }, 1500);
});
