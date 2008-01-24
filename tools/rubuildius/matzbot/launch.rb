#!/usr/bin/env ruby

$:.unshift "lib"
require 'rubygems'
require 'choice'
require_gem 'choice', '>= 0.1.2'
require 'matzbot'

Choice.options do
  header ""
  header "Specific options:" 

  options  :server => { 
    :short => '-s', :long => '--server SERVER',
    :desc => ["The irc server to connect to.", "(default irc.freenode.org)"], :default => 'irc.freenode.org'
  },

  :port => {
    :short => '-p', :long => '--port PORT', :desc => ["The irc server's port.", "(default 6667)"],
    :default => 6667, :cast => Integer
  },

  :nick => {
    :short => '-n', :long => '--nick NICK', :desc => ["The irc bot's nick.", "(default matz)"],
    :default => 'matz' 
  },

  :password => {
    :short => '-w', :long => '--password PASSWORD', :desc => "Your nick's password, if registered."
  },

  :channel => {
    :short => '-c', :long => '--channel CHANNEL', :desc => ["The channel to join on connect -- without a hash.", "(default polymorphs)"],
    :default => 'polymorphs' 
  },

  :user => {
    :short => '-u', :long => '--user USER', :desc => ["The irc bot's username.", "(default matz)"], :default => 'matz' 
  },

  :name => {
    :short => '-m', :long => '--name NAME', :desc => ["The irc bot's name.", "(default Matz)"], :default => 'Matz' 
  },

  :daemonize => {
    :short => '-d', :long => '--daemon',
    :desc => ["Run as a daemon - pass it nothing, start, stop, or restart.", "If you pass nothing, `start' is assumed."]
  },
  
  :only_when_addressed => {
    :short => '-a', :long => '--addressed',
    :desc => ["Only speak if being addressed.", "(default false)"]
  }
end

MatzBot::Session.load unless Choice.choices[:daemonize] == 'stop'

require 'daemon'

if Choice.choices[:daemonize]
  MatzBot::Daemonize.daemonize(Choice.choices.merge('daemonize' => Choice.choices[:daemonize] == 'stop' ? 'stop' : 'start'))
else
  MatzBot::Daemonize.store
  begin
    MatzBot::Client.start(Choice.choices)
  ensure
    MatzBot::Session.save
  end
end
