$(document).ready(function() {
  $("#twitter").getTwitter({
    userName: "rubinius",
    numTweets: 6,
    loaderText: "Loading @Rubinius Tweets&hellip;",
    slideIn: false,
    showHeading: false,
    headingText: "Latest Tweets",
    showProfileLink: true
  });
});
