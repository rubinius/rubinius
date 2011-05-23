/*  from http://la.ma.la/blog/diary_200511041713.htm - hotkey.js */

function HotKey(element){
	this.target = element || document;
	this._keyfunc = {};
	this.init();
}
HotKey.kc2char = function(kc){
	var between = function(a,b){
		return a <= kc && kc <= b
	}
	var _32_40 = "space pageup pagedown end home left up right down".split(" ");
	var kt = {
		8  : "back",
		9  : "tab"  ,
		13 : "enter",
		16 : "shift",
		17 : "ctrl",
		46 : "delete"
	};
	return (
		between(65,90)  ? String.fromCharCode(kc+32) : // a-z
		between(48,57)  ? String.fromCharCode(kc) :    // 0-9
		between(96,105) ? String.fromCharCode(kc-48) : // num 0-9
		between(32,40)  ? _32_40[kc-32] :
		kt.hasOwnProperty(kc) ? kt[kc] :
		null
	)
}
HotKey.prototype.ignore = /input|textarea/i;
HotKey.prototype.init = function(){
	var self = this;
	var listener = function(e){
		self.onkeydown(e)
	};
	if(this.target.addEventListener){
		this.target.addEventListener("keydown", listener, true);
	}else{
		this.target.attachEvent("onkeydown", listener)
	}
}
HotKey.prototype.onkeydown = function(e){
	var tag = (e.target || e.srcElement).tagName;
	if(this.ignore.test(tag)) return;
	var input = HotKey.kc2char(e.keyCode);

	if(e.shiftKey && input.length == 1){
		input = input.toUpperCase()
	}
	if(e.altKey) input = "A" + input;
	if(e.ctrlKey) input = "C" + input;

	if(this._keyfunc.hasOwnProperty(input)){
		this._keyfunc[input].call(this,e)
	}
}
HotKey.prototype.sendKey = function(key){
	this._keyfunc[key] && this._keyfunc[key]()
}
HotKey.prototype.add = function(key,func){
	if(key.constructor == Array){
		for(var i=0;i<key.length;i++)
			this._keyfunc[key[i]] = func;
	}else{
		this._keyfunc[key] = func;
	}
}
HotKey.prototype.remove = function(key){
	if(key.constructor == Array){
		for(var i=0;i<key.length;i++)
			this._keyfunc[key[i]] = function () {};
	}else{
		this._keyfunc[key] = function () {};
	}
}


/* https://github.com/hiddenloop/paging_keys_js - inspired by navigation on ffffound.com UI */
/* by Matthew Hutchinson - hiddenloop.com */

var pagingKeys = function() {

	// settings
	var config = {
    nodeSelector:        '.stripe',  // used to select each item on the page and place in the map (must be a link)
    prevPageSelector:    '.prev_page',                // link on this element should always jump to prev page a.prev_page (must be a link)
		nextPageSelector:    '.next_page',                // link on this element should always jump to next page a.next_page (must be a link)
		pagingNavId:         'paging-nav',                // dom id of the floating page navigation element
		keyNext:             'j',                         // hot keys used
		keyPrev:             'k',
		keyNextPage:         'h',
		keyPrevPage:         'l',
		keyRefresh:          'r',
		additionalBodyClass: 'paging-keys',               // this class is assigned to the page body on load
		bottomAnchor:        'bottom'                     // the name of the anchor (without #) at end of page, e.g. set on last post on the page
  };

	var item_map        = [];
	var asset_loaded    = false;
	var hot_key         = false;
	var disable_hot_key = false;

	// abstraction layer start <modify these methods for other library support>
	// jquery

	function windowScrollInit() {
		$(window).scroll(function () {
      positionNav();
    });
	}

	function getWindowBounds() {
		return {
	    'w': $(window).width(),
	    'h': $(window).height(),
	    'x': $(window).scrollLeft(),
	    'y': $(window).scrollTop()
	  };
	}

	function getEl(selector) {
		return $(selector);
	}

	function addItemToMap(n) {
	  var pos = $(n).offset();
	  item_map.push({id: n.id, y: Math.round(pos.top) - 20});
	}

	function setNavCSS() {
		$('#'+config.pagingNavId).css({ position: 'absolute', right: '10px', top: (getScrollTop()+10)+'px' });
	}

	function getScrollTop() {
		return $(window).scrollTop();
	}

	function isIE() {
		return $.browser.msie;
	}

	function isWebKit() {
		return $.browser.safari;
	}

	function init() {
		$(window).load(setupPagingKeys);
		windowScrollInit();
	}

	// abstraction layer end


	function setupPagingKeys() {
	  // TODO: escape/return when incompatible browser found
		var b = document.body;
		b.className = b.className ? b.className + (' '+config.additionalBodyClass) : config.additionalBodyClass;
		buildItemMap();
		positionNav();
		initHotKeys();
	}

	// 'prev' and 'next' are used to identify items and their position in the map
	function buildItemMap() {
	  asset_loaded = false;

	  if (getEl(config.prevPageSelector)[0]) {
		  if(getEl(config.prevPageSelector)[0].href)
	     item_map.push({id: 'prev', y: 0});
	  }
	  else
	    item_map.push({id: null, y: 0});

	  var nodes = getEl(config.nodeSelector);
	  for (var i = 0; i < nodes.length; i++) {
      addItemToMap(nodes[i]);
	  }
    // sort based on page Y postion
	  item_map.sort(function(a, b) {
	    return a.y - b.y;
	  });

	  var last = item_map.length - 1;
		if(getEl(config.nextPageSelector)[0]) {
	  	if(getEl(config.nextPageSelector)[0].href)
	    	item_map.push({id: 'next', y: document.body.scrollHeight});
	  }
	  asset_loaded = true;
	}

	// optional, repositioning of the floating navigation element
	function positionNav() {
		if($(config.pagingNavId))
			setNavCSS();
	}

  // enable hotkeys
	function initHotKeys() {
		try {
		  hot_key = new HotKey();
		}
		catch (e) {
		  alert('Oops, paging_keys requires HotKeys.js (http://la.ma.la/blog/diary_200511041713.htm)');
		  alert(e);
		}
		if (hot_key) {
		  hot_key.add(config.keyNext, function() { moveToItem(1); });
		  hot_key.add(config.keyPrev, function() { moveToItem(-1); });
		  hot_key.add(config.keyNextPage, function() { movePage(1); });
		  hot_key.add(config.keyPrevPage, function() { movePage(-1); });
		  hot_key.add(config.keyRefresh, function() { location.reload(); });
		}
	}

	function redirect(href) {
		/* fix IE */
	  if (isIE()) {
	    var a = document.createElement('a');
	    a.style.display = 'none';
	    a.href = href;
	    document.body.appendChild(a);
	    a.click();
	  }
	  else {
	    location.href = href;
	  }
	}

	function disableHotKeys() {
	  if (hot_key) {
	    disable_hot_key = true;
	    hot_key.remove(config.keyNext);
	    hot_key.remove(config.keyPrev);
	    hot_key.remove(config.keyNextPage);
	    hot_key.remove(config.keyPrevPage);
	  }
	}

	function moveToItem(delta, p) {
	  if (!asset_loaded)
	    return false;

	  if (p == null)
	    p = currentItem(delta);
	  var old_y = getScrollTop();

	  if (p) {
	    if (p.id == 'prev' || p.id == 'next') {
	      if (p.id == 'next')
	        movePageNext();
	      else
	        movePagePrev();
	      return false;
	    }
			window.scrollTo(0, p.y);

	    if((delta > 0) && (old_y == getScrollTop()))
	      movePage(1);
	  }
	  return true;
	}

	function currentItem(delta, y) {
	  if (y == null)
	    y = getScrollTop();

	  var p = item_map.length - 1;
	  for (var i = 0; i < item_map.length; i++) {
	    if (y < item_map[i].y) {
	      p = i - 1;
	      break;
	    }
	  }

	  if ((delta < 0 && item_map[p] && item_map[p].y == y) || 0 < delta)
	    p += delta;
	  else if (getWindowBounds().h + getScrollTop() == document.body.scrollHeight && 0 < delta)
	    p++;

	  p = Math.max(p, 0);
	  return item_map[p];
	}

  // determine current position in the document based on viewport and scrolling
	function whereAmI() {
	  var st = document.body.scrollTop;
	  var sl = document.body.scrollLeft;
	  var sh = document.body.scrollHeight;
	  var ch = 0;

	  if (isWebKit())
	    ch = window.innerHeight;
	  else
	    ch = document.body.clientHeight;

	  return {
      'top': st,
      'left': sl,
      'height': sh,
      'clientHeight': ch,
      'is_at_top': st == 0 && sl == 0,
      'is_at_last': st + ch == sh && sl == 0
	  }
	}

	function movePage(delta) {
	  var p = whereAmI();
	  if (delta < 0) {
	    if (p.is_at_top)
	      movePagePrev();
	    else
	      window.scroll(0, 0);
	  }
	  else {
	    if (p.is_at_last)
	      movePageNext();
	    else
	      window.scroll(0, p.height);
	  }
	}

	function movePageNext() {
		if (getEl(config.nextPageSelector)[0]) {
	  	if (getEl(config.nextPageSelector)[0].href != null) {
	    	redirect(getEl(config.nextPageSelector)[0].href);
	    	disableHotKeys();
	    	return true;
			}
	  }
	  else { return false; }
	}

	function movePagePrev() {
		if (getEl(config.prevPageSelector)[0]) {
	 	 if (getEl(config.prevPageSelector)[0].href != null) {
		    redirect(getEl(config.prevPageSelector)[0].href+'#'+config.bottomAnchor);
		    disableHotKeys();
		    return true;
		  }
	  }
	  else { return false; }
	}

	// return public pointers to the private methods and properties you want to reveal
  return {
    init:init,
    moveToItem:moveToItem,
    movePage:movePage,
    currentItem:currentItem,
    config:config,
    item_map:item_map
  }
}();

pagingKeys.init();