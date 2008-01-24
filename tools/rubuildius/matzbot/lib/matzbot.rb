$:.unshift File.dirname(__FILE__)
require 'session'
require 'client'
require 'commands'

module MatzBot
  extend self

  @loaded_plugins ||= {}

  def reload!
    load File.join(File.expand_path(File.dirname(__FILE__)), 'session.rb')
    load File.join(File.expand_path(File.dirname(__FILE__)), 'client.rb')
    reload_plugins!
  end

  def reload_plugins!
    commands_file = File.join(File.expand_path(File.dirname(__FILE__)), 'commands.rb')
    if [commands_file, plugin_files].flatten.any? { |f| plugin_changed? f }
      Commands.send(:reload_filters!)
      MatzBot.send(:remove_const, :Commands)
      load commands_file
      load_plugins
    end
  end

  def load_plugins
    plugin_files.each do |file|
      puts "Loading #{file}..."
      begin
        touch_plugin(file)
        load file 
      rescue Object => e
        puts "Unable to load #{file}, disabled."
        p e
      end
    end
  end

  def plugin_files
    [dot_matz_bot, 'plugins'].map do |directory|
      next unless File.exists? directory
      Dir[File.join(directory, '*.rb')]
    end.flatten.compact
  end

  def dot_matz_bot
    @dot_matz_bot ||= 
      if PLATFORM =~ /win32/ 
        dot_matz_bot = ENV['HOMEDRIVE'] + ENV['HOMEPATH']
        dot_matz_bot = File.join(home, 'MatzBot')
      else
        dot_matz_bot = File.join(File.expand_path("~/matzbot/"), "configuration")
      end
  end

  def plugin_changed?(file)
    touch_plugin(file) if !@loaded_plugins[file] || @loaded_plugins[file] < File.mtime(file)
  end

  def touch_plugin(file)
    @loaded_plugins[file] = File.mtime(file)
  end
end
