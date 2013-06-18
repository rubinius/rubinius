---
layout: post
title: Heroku (Cedar) + Rubinius 2 + Puma 2 in Three Easy Steps
author: Shane Becker
---

### tl;dr

It's now **[very easy](https://github.com/rubinius/heroku-rbx-puma-rails-app)** to get a [Rails app](https://devcenter.heroku.com/articles/rails3) on [Heroku's Cedar Stack](https://devcenter.heroku.com/articles/cedar) running on [Rubinius 2.0](http://rubini.us "Rubinius : Use Ruby&#8482;") and [Puma 2.0](http://puma.io "A Modern, Concurrent Web Server for Ruby - Puma").


### The Backstory

The fine folks at Heroku have been adding support for using [Bundler](http://gembundler.com "Bundler: The best way to manage a Ruby application's gems") to [specify which version of Ruby](https://devcenter.heroku.com/articles/ruby-versions) your app uses and even [to specify which Ruby](https://blog.heroku.com/archives/2012/12/13/run_jruby_on_heroku_right_now) it uses.

I've checked in on the progress of using Rubinius with this same method every couple few months for the past year or so. I was always met with the same results... <q>"not quite yet, we're working on it"</q>.

Last night I was getting mentally packed for [Waza](https://waza.heroku.com/2013) later this week and decided to check in on the state of things so I could ask the right people the right questions in person when we're all together. Turns out that sometime between last time and this time, they've got things working. Boom diggity! And it's pretty ding-dang easy too! Double boom diggity!


### The Build Up

I'm going to assume that if you want to use Rubinius 2.0 that you also have the good sense to want to use Puma 2.0. There are three very simple steps to get up and running with Rubinius 2.0 + Puma 2.0.

#### Step 1

Add the Puma gem to your `Gemfile`.

<script src="https://gist.github.com/veganstraightedge/5041441.js"></script>


#### Step 2

Tell Heroku to use Puma as your web server in your `Procfile`.

<script src="https://gist.github.com/veganstraightedge/0135a61335bc76b1d9d5.js"></script>

#### Step 3

Specify Rubinius as your Ruby engine in your `Gemfile`.

<script src="https://gist.github.com/veganstraightedge/1fb7ff88e74567c6e2e6.js"></script>


### The Breakdown

Once you've done those things, your workflow is the same as before.
`bundle update && git commit -am "Double boom diggity!" && git push heroku master`

If you're changing an existing app from Heroku's default Ruby (MRI 1.9.x) to Rubinius, you'll see a message like this when you `git push heroku master`.

<script src="https://gist.github.com/veganstraightedge/5041986.js"></script>

The important lines are `Old: ruby 1.9.3` and `New: rubinius 2.0.0.rc1`. After that, everything the same as before. The bundling starts, etc.


### The Outro

That's it. You should now be up and running with Rubinius and Puma!

If you have problems, say something in the comments with a link to a gist with your problem output. If you succeed and get up and running in production, let us in the comments too. Tell the world that you love Rubinius/Puma.

I threw together this [quick and dirty Rails app](https://github.com/rubinius/heroku-rbx-puma-rails-app) from scratch deployed to Heroku to show all this in action. [http://heroku-rbx-puma-rails-app.herokuapp.com](http://heroku-rbx-puma-rails-app.herokuapp.com "Using Rubinius &amp; Puma on Heroku"). I also migrated an existing MRI 1.9.3 app ( [The Farmhouse site](http://farmhouse.la "The Farmhouse in Hollywood, California") ) to Rubinius 2.0 to show the migration is easy too.


### The Thank Yous

None of this would be possible without the hard work of [Carl Lerche](https://github.com/carllerche), [Yehuda Katz](https://github.com/wycats/), [André Arko](https://github.com/indirect/), [Terrence Lee](https://github.com/hone) and the other contributors to [Bundler](https://github.com/carlhuda/bundler). And a special thanks to Terrence Lee for his dutiful stewardship of the [Heroku Ruby Buildpack](https://github.com/heroku/heroku-buildpack-ruby/). Once again, the awesome powers of Open Source make great things possible.


### The Footnotes

As of this writing "Rubinius 2.0" actually means "Rubinius 2.0.0.rc1" and "Puma 2.0" actually means "Puma 2.0.1". The net result is the same though.

