(function($) {
	/*
		jquery.twitter.js v1.0
		Last updated: 26 October 2008

		Created by Damien du Toit
		http://coda.co.za/blog/2008/10/26/jquery-plugin-for-twitter

		Licensed under a Creative Commons Attribution-Non-Commercial 3.0 Unported License
		http://creativecommons.org/licenses/by-nc/3.0/
	*/

	$.fn.getTwitter = function(options) {
		var o = $.extend({}, $.fn.getTwitter.defaults, options);

		// hide container element
		$(this).hide();

		// add heading to container element
		if (o.showHeading) {
			$(this).append('<h2>'+o.headingText+'</h2>');
		}

		// add twitter list to container element
		$(this).append('<ul id="twitter_update_list"><li></li></ul>');

		// hide twitter list
		$("ul#twitter_update_list").hide();

		// add preLoader to container element
		var pl = $('<p id="'+o.preloaderId+'">'+o.loaderText+'</p>');
		$(this).append(pl);

		// add Twitter profile link to container element
		if (o.showProfileLink) {
			$(this).append('<a id="profileLink" href="http://twitter.com/'+o.userName+'">http://twitter.com/'+o.userName+'</a>');
		}

		// show container element
		$(this).show();

		$.getScript("http://twitter.com/javascripts/blogger.js");
		$.getScript("http://twitter.com/statuses/user_timeline/"+o.userName+".json?callback=twitterCallback2&count="+o.numTweets, function() {
			// remove preLoader from container element
			$(pl).remove();

			// show twitter list
			if (o.slideIn) {
				$("ul#twitter_update_list").slideDown(1000);
			}
			else {
				$("ul#twitter_update_list").show();
			}

			// give first list item a special class
			$("ul#twitter_update_list li:first").addClass("firstTweet");

			// give last list item a special class
			$("ul#twitter_update_list li:last").addClass("lastTweet");
		});
	};

	// plugin defaults
	$.fn.getTwitter.defaults = {
		userName: null,
		numTweets: 5,
		preloaderId: "preloader",
		loaderText: "Loading tweets...",
		slideIn: false,
		showHeading: true,
		headingText: "Latest Tweets",
		showProfileLink: true
	};
})(jQuery);