# http://www.bigbold.com/snippets/posts/show/2265
# (but hacked)
require 'fileutils'

module MatzBot 
  module Daemonize
    extend self

    WorkingDirectory = File.expand_path(FileUtils.pwd)  

    def pid_fn
      File.join(WorkingDirectory, "matzbot.pid")
    end
    
    def daemonize(config)
      case config[:daemonize]
      when 'start'
        start(config)
      when 'stop'
        stop
      when 'restart'
        stop
        start(config)
      else
        puts "Invalid command. Please specify start, stop or restart."
        exit
      end
    end
    
    def start(config)
      fork do
        Process.setsid
        exit if fork
        store(Process.pid)
        Dir.chdir WorkingDirectory
        File.umask 0000
        STDIN.reopen "/dev/null"
        STDOUT.reopen "/dev/null", "a"
        STDERR.reopen STDOUT
        trap("TERM") do 
          MatzBot::Session.save
          exit
        end
        begin
          Client.start(config)
        ensure
          stop
        end
      end
    end
  
    def stop
      unless File.file? pid_fn
        puts "Pid file not found. Is the daemon started?"
        exit
      end
      pid = recall
      FileUtils.rm pid_fn
      puts "Process killed."
      Process.kill("TERM", pid) rescue nil
    end

    def store(pid=Process.pid)
      if File.file? pid_fn
        puts "** Already started with PID #{File.read(pid_fn)}"
        exit!
      else
        File.open(pid_fn, 'w') { |f| f << pid }
      end
    end
    
    def recall
      IO.read(pid_fn).to_i rescue nil
    end
  end
end
