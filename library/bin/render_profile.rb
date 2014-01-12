require 'rubygems'
require 'json'

def header(title)
  code = <<-HTML
<html><head>
<meta http-equiv="content-type" content="text/html; charset=utf-8">
<title>#{title}</title>
<style type="text/css">
<!--
body {
    font-size:70%;
    padding:0px;
    margin:5px;
    margin-right:0px;
    margin-left:0px;
    background: #ffffff;
}
ul {
    margin-left:0px;
    margin-top:0px;
    margin-bottom:0px;
    padding-left:0px;
    list-style-type:none;
}
li {
    margin-left:11px;
    padding:0px;
    white-space:nowrap;
    border-top:1px solid #cccccc;
    border-left:1px solid #cccccc;
    border-bottom:none;
}
.thread {
    margin-left:11px;
    background:#708090;
    padding-top:3px;
    padding-left:12px;
    padding-bottom:2px;
    border-left:1px solid #CCCCCC;
    border-top:1px solid #CCCCCC;
    font-weight:bold;
}
.hidden {
    display:none;
    width:0px;
    height:0px;
    margin:0px;
    padding:0px;
    border-style:none;
}
.color01 { background:#adbdeb }
.color05 { background:#9daddb }
.colorS { background:white }
.color0 { background:#8d9dcb }
.color1 { background:#89bccb }
.color2 { background:#56e3e7 }
.color3 { background:#32cd70 }
.color4 { background:#a3d53c }
.color5 { background:#c4cb34 }
.color6 { background:#dcb66d }
.color7 { background:#cda59e }
.color8 { background:#be9d9c }
.color9 { background:#cf947a }
#commands {
    font-size:10pt;
    padding:10px;
    margin-left:11px;
    margin-bottom:0px;
    margin-top:0px;
    background:#708090;
    border-top:1px solid #cccccc;
    border-left:1px solid #cccccc;
    border-bottom:none;
}
#titlebar {
    font-size:10pt;
    padding:10px;
    margin-left:11px;
    margin-bottom:0px;
    margin-top:10px;
    background:#8090a0;
    border-top:1px solid #cccccc;
    border-left:1px solid #cccccc;
    border-bottom:none;
}
#help {
    font-size:10pt;
    padding:10px;
    margin-left:11px;
    margin-bottom:0px;
    margin-top:0px;
    background:#8090a0;
    display:none;
    border-top:1px solid #cccccc;
    border-left:1px solid #cccccc;
    border-bottom:none;
}
#sentinel {
    height: 400px;
    margin-left:11px;
    background:#8090a0;
    border-top:1px solid #cccccc;
    border-left:1px solid #cccccc;
    border-bottom:none;
 }
input { margin-left:10px; }
-->
</style>
<script type="text/javascript">
/*
   Copyright (C) 2005,2009  Stefan Kaes
   skaes@railsexpress.de
*/

function rootNode() {
  return currentThread;
}

function hideUL(node) {
  var lis = node.childNodes
  var l = lis.length;
  for (var i=0; i < l ; i++ ) {
    hideLI(lis[i]);
  }
}

function showUL(node) {
  var lis = node.childNodes;
  var l = lis.length;
  for (var i=0; i < l ; i++ ) {
    showLI(lis[i]);
  }
}

function findUlChild(li){
  var ul = li.childNodes[2];
  while (ul && ul.nodeName != "UL") {
    ul = ul.nextSibling;
  }
  return ul;
}

function isLeafNode(li) {
  var img = li.firstChild;
  return (img.src.indexOf('empty.png') > -1);
}

function hideLI(li) {
  if (isLeafNode(li))
    return;

  var img = li.firstChild;
  img.src = 'http://asset.rubini.us/plus.png';

  var ul = findUlChild(li);
  if (ul) {
    ul.style.display = 'none';
    hideUL(ul);
  }
}

function showLI(li) {
  if (isLeafNode(li))
    return;

  var img = li.firstChild;
  img.src = 'http://asset.rubini.us/minus.png';

  var ul = findUlChild(li);
  if (ul) {
    ul.style.display = 'block';
    showUL(ul);
  }
}

function toggleLI(li) {
  var img = li.firstChild;
  if (img.src.indexOf("minus.png")>-1)
    hideLI(li);
  else {
    if (img.src.indexOf("plus.png")>-1)
      showLI(li);
  }
}

function aboveThreshold(text, threshold) {
  var match = text.match(/\d+[.,]\d+/);
  return (match && parseFloat(match[0].replace(/,/, '.'))>=threshold);
}

function setThresholdLI(li, threshold) {
  var img = li.firstChild;
  var text = img.nextSibling;
  var ul = findUlChild(li);

  var visible = aboveThreshold(text.nodeValue, threshold) ? 1 : 0;

  var count = 0;
  if (ul) {
    count = setThresholdUL(ul, threshold);
  }
  if (count>0) {
    img.src = 'http://asset.rubini.us/minus.png';
  }
  else {
    img.src = 'http://asset.rubini.us/empty.png';
  }
  if (visible) {
    li.style.display = 'block'
  }
  else {
    li.style.display = 'none'
  }
  return visible;
}

function setThresholdUL(node, threshold) {
  var lis = node.childNodes;
  var l = lis.length;

  var count = 0;
  for ( var i = 0; i < l ; i++ ) {
    count = count + setThresholdLI(lis[i], threshold);
  }

  var visible = (count > 0) ? 1 : 0;
  if (visible) {
    node.style.display = 'block';
  }
  else {
    node.style.display = 'none';
  }
  return visible;
}

function toggleChildren(img, event) {
  event.cancelBubble=true;

  if (img.src.indexOf('empty.png') > -1)
    return;

  var minus = (img.src.indexOf('minus.png') > -1);

  if (minus) {
    img.src = "http://asset.rubini.us/plus.png";
  }
  else
    img.src = "http://asset.rubini.us/minus.png";

  var li = img.parentNode;
  var ul = findUlChild(li);
  if (ul) {
    if (minus)
      ul.style.display = 'none';
    else
      ul.style.display = 'block';
  }
  if (minus)
    moveSelectionIfNecessary(li);
}

function showChildren(li) {
  var img = li.firstChild;
  if (img.src.indexOf('empty.png') > -1)
    return;
  img.src = "http://asset.rubini.us/minus.png";

  var ul = findUlChild(li);
  if (ul) {
    ul.style.display = 'block';
  }
}

function setThreshold() {
 var tv = document.getElementById("threshold").value;
 if (tv.match(/[0-9]+([.,][0-9]+)?/)) {
   var f = parseFloat(tv.replace(/,/, '.'));
   var threads = document.getElementsByName("thread");
   var l = threads.length;
   for ( var i = 0; i < l ; i++ ) {
     setThresholdUL(threads[i], f);
   }
   var p = selectedNode;
   while (p && p.style.display=='none')
     p=p.parentNode.parentNode;
   if (p && p.nodeName=="LI")
    selectNode(p);
 }
 else {
   alert("Please specify a decimal number as threshold value!");
 }
}

function collapseAll(event) {
  event.cancelBubble=true;
  var threads = document.getElementsByName("thread");
  var l = threads.length;
  for ( var i = 0; i < l ; i++ ) {
    hideUL(threads[i]);
  }
  selectNode(rootNode(), null);
}

function expandAll(event) {
  event.cancelBubble=true;
  var threads = document.getElementsByName("thread");
  var l = threads.length;
  for ( var i = 0; i < l ; i++ ) {
    showUL(threads[i]);
  }
}

function toggleHelp(node) {
  var help = document.getElementById("help");
  if (node.value == "Show Help") {
    node.value = "Hide Help";
    help.style.display = 'block';
  }
  else {
    node.value = "Show Help";
    help.style.display = 'none';
  }
}

var selectedNode = null;
var selectedColor = null;
var selectedThread = null;

function descendentOf(a,b){
  while (a!=b && b!=null)
    b=b.parentNode;
  return (a==b);
}

function moveSelectionIfNecessary(node){
  if (descendentOf(node, selectedNode))
    selectNode(node, null);
}

function selectNode(node, event) {
  if (event) {
    event.cancelBubble = true;
    thread = findThread(node);
    selectThread(thread);
  }
  if (selectedNode) {
    selectedNode.style.background = selectedColor;
  }
  selectedNode = node;
  selectedColor = node.style.background;
  selectedNode.style.background = "red";
  selectedNode.scrollIntoView();
  window.scrollBy(0,-400);
}

function moveUp(){
  var p = selectedNode.previousSibling;
  while (p && p.style.display == 'none')
    p = p.previousSibling;
  if (p && p.nodeName == "LI") {
    selectNode(p, null);
  }
}

function moveDown(){
  var p = selectedNode.nextSibling;
  while (p && p.style.display == 'none')
    p = p.nextSibling;
  if (p && p.nodeName == "LI") {
    selectNode(p, null);
  }
}

function moveLeft(){
  var p = selectedNode.parentNode.parentNode;
  if (p && p.nodeName=="LI") {
    selectNode(p, null);
  }
}

function moveRight(){
  if (!isLeafNode(selectedNode)) {
    showChildren(selectedNode);
    var ul = findUlChild(selectedNode);
    if (ul) {
      selectNode(ul.firstChild, null);
    }
  }
}

function moveForward(){
  if (isLeafNode(selectedNode)) {
    var p = selectedNode;
    while ((p.nextSibling == null || p.nextSibling.style.display=='none') && p.nodeName=="LI") {
      p = p.parentNode.parentNode;
    }
    if (p.nodeName=="LI")
      selectNode(p.nextSibling, null);
  }
  else {
    moveRight();
  }
}

function isExpandedNode(li){
  var img = li.firstChild;
  return(img.src.indexOf('minus.png')>-1);
}

function moveBackward(){
  var p = selectedNode;
  var q = p.previousSibling;
  while (q != null && q.style.display=='none')
    q = q.previousSibling;
  if (q == null) {
    p = p.parentNode.parentNode;
  } else {
    while (!isLeafNode(q) && isExpandedNode(q)) {
      q = findUlChild(q).lastChild;
      while (q.style.display=='none')
        q = q.previousSibling;
    }
    p = q;
  }
  if (p.nodeName=="LI")
    selectNode(p, null);
}

function moveHome() {
  selectNode(currentThread);
}

var currentThreadIndex = null;

function findThread(node){
  while (node && node.parentNode.nodeName!="BODY") {
    node = node.parentNode;
  }
  return node.firstChild;
}

function selectThread(node){
  var threads = document.getElementsByName("thread");
  currentThread = node;
  for (var i=0; i<threads.length; i++) {
    if (threads[i]==currentThread.parentNode)
      currentThreadIndex = i;
  }
}

function nextThread(){
  var threads = document.getElementsByName("thread");
  if (currentThreadIndex==threads.length-1)
    currentThreadIndex = 0;
  else
    currentThreadIndex += 1
  currentThread = threads[currentThreadIndex].firstChild;
  selectNode(currentThread, null);
}

function previousThread(){
  var threads = document.getElementsByName("thread");
  if (currentThreadIndex==0)
    currentThreadIndex = threads.length-1;
  else
    currentThreadIndex -= 1
  currentThread = threads[currentThreadIndex].firstChild;
  selectNode(currentThread, null);
}

function switchThread(node, event){
  event.cancelBubble = true;
  selectThread(node.nextSibling.firstChild);
  selectNode(currentThread, null);
}

function handleKeyEvent(event){
  var code = event.charCode ? event.charCode : event.keyCode;
  var str = String.fromCharCode(code);
  switch (str) {
    case "a": moveLeft();  break;
    case "s": moveDown();  break;
    case "d": moveRight(); break;
    case "w": moveUp();    break;
    case "f": moveForward(); break;
    case "b": moveBackward(); break;
    case "x": toggleChildren(selectedNode.firstChild, event); break;
    case "*": toggleLI(selectedNode); break;
    case "n": nextThread(); break;
    case "h": moveHome(); break;
    case "p": previousThread(); break;
  }
}
document.onkeypress=function(event){ handleKeyEvent(event) };

window.onload=function(){
  var images = document.getElementsByTagName("img");
  for (var i=0; i<images.length; i++) {
    var img = images[i];
    if (img.className == "toggle") {
      img.onclick = function(event){ toggleChildren(this, event); };
    }
  }
  var divs = document.getElementsByTagName("div");
  for (i=0; i<divs.length; i++) {
    var div = divs[i];
    if (div.className == "thread")
      div.onclick = function(event){ switchThread(this, event) };
  }
  var lis = document.getElementsByTagName("li");
  for (var i=0; i<lis.length; i++) {
    lis[i].onclick = function(event){ selectNode(this, event); };
  }
  var threads = document.getElementsByName("thread");
  currentThreadIndex = 0;
  currentThread = threads[0].firstChild;
  selectNode(currentThread, null);
}
</script>
</head><body>
<div id="titlebar">
Call tree for application <b>/Users/evan/.gem/ruby/1.8/bin/ruby-prof </b><br/>
Generated on Mon Mar 28 20:42:29 -0700 2011 with options {:min_percent=&gt;0}<br/>
</div>
<div id="commands">
<span style="font-size: 11pt; font-weight: bold;">Threshold:</span>
<input value="1.0" size="3" id="threshold" type="text">
<input value="Apply" onclick="setThreshold();" type="submit">
<input value="Expand All" onclick="expandAll(event);" type="submit">
<input value="Collapse All" onclick="collapseAll(event);" type="submit">
<input value="Show Help" onclick="toggleHelp(this);" type="submit">
</div>
<div style="display: none;" id="help">
<img src="http://asset.rubini.us/empty.png"> Enter a decimal value <i>d</i> into the threshold field and click "Apply"
to hide all nodes marked with time values lower than <i>d</i>.<br>
<img src="http://asset.rubini.us/empty.png"> Click on "Expand All" for full tree expansion.<br>
<img src="http://asset.rubini.us/empty.png"> Click on "Collapse All" to show only top level nodes.<br>
<img src="http://asset.rubini.us/empty.png"> Use a, s, d, w as in Quake or Urban Terror to navigate the tree.<br>
<img src="http://asset.rubini.us/empty.png"> Use f and b to navigate the tree in preorder forward and backwards.<br>
<img src="http://asset.rubini.us/empty.png"> Use x to toggle visibility of a subtree.<br>
<img src="http://asset.rubini.us/empty.png"> Use * to expand/collapse a whole subtree.<br>
<img src="http://asset.rubini.us/empty.png"> Use h to navigate to thread root.<br>
<img src="http://asset.rubini.us/empty.png"> Use n and p to navigate between threads.<br>
<img src="http://asset.rubini.us/empty.png"> Click on background to move focus to a subtree.<br>
</div>
  HTML

  return code
end

def print_node(f, id, data, depth=0)
  node = data["nodes"][id.to_s]

  unless node
    puts "Missing node: #{id}"
    return
  end

  prec = "%.2f" % [100.0 * (node["total"].to_f / data["runtime"].to_f)]

  meth = data["methods"][node["method"].to_s]
  name = "<a href=\"txmt://open?url=file://#{meth['file']}&line=#{meth['line']}\">#{meth['name']}</a> (#{node['called']}/#{meth['called']})"

  if prec.to_f <= 1.0
    color = "S"
  else
    color = prec.to_i / 10
  end

  child = node['sub_nodes'].map { |x| data['nodes'][x.to_s] }.inject(0) { |a,n| a + n['total'] }
  s = node['total'] - child

  self_prec = "%.2f" % [100.0 * (s / data['runtime'].to_f)]

  if node["sub_nodes"].empty?
    f.puts %Q!<li class="color#{color}" style="display:block"><img src="http://asset.rubini.us/empty.png"> #{prec}% (#{self_prec}%) #{name}!
  else
    if depth > 20
      f.puts %Q!<li class="color#{color}" style="display:block"><img class="toggle" src="http://asset.rubini.us/plus.png"> #{prec}% (#{self_prec}%) #{name}!
      f.puts "<ul style=\"display:none\">"
    else
      f.puts %Q!<li class="color#{color}" style="display:block"><img class="toggle" src="http://asset.rubini.us/minus.png"> #{prec}% (#{self_prec}%) #{name}!
      f.puts "<ul>"
    end

    subs = node["sub_nodes"].sort_by do |s|
      if n = data["nodes"][s.to_s]
        n["total"].to_f / data["runtime"]
      else
        0
      end
    end

    subs.reverse_each do |s_id|
      print_node(f, s_id, data, depth + 1)
    end
    f.puts "</ul>"
  end

  f.puts "</li>"
end

file = ARGV.shift

unless File.exists? file
  puts "File '#{file}' does not exist"
  exit 1
end

output = ARGV.shift

puts "Rendering profiling from '#{file}' to '#{output}'"

data = JSON.load File.read(file)

File.open output, "w" do |f|
  f << header("Profiling from '#{file}'")

  data.each do |th_data|
    f.puts "<div class=\"thread\">Thread #{th_data['thread_id']}</div>"
    f.puts "<ul name=\"thread\">"
    th_data["roots"].each do |n_id|
      print_node(f, n_id, th_data)
    end
    f.puts "</ul>"
  end

  f.puts "<div id=\"sentinel\"></div></body></html>"
end


