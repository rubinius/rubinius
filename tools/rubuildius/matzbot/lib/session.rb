require 'fileutils'

module MatzBot
  module Session
    extend self

    def session_file
      File.join(home_dir, 'session.yml')
    end

    def home_dir
      MatzBot.dot_matz_bot
    end

    def session
      $session ||= {}
    end

    def save
      FileUtils.mkdir(home_dir) unless File.exists? home_dir

      File.open(session_file, 'w') do |f|
        session_marshalled = session.dup
        marshalled = {}

        session_marshalled.each do |key, value|
          callback(:session_before_save, key)
          begin
            marshalled[key] = Marshal.dump(value)
          rescue  
            # don't dump things we cant marshal
          end
        end
        # save
        f.puts Marshal.dump(marshalled)
      end
    rescue
      puts "Problem saving session file!"
    end

    def load
      if File.exist? session_file
        Marshal.load(open(session_file).read).each do |key, value|
          session[key] = Marshal.load(value)
          puts "Loaded session key: #{key}."
          callback(:session_after_load, key)
        end
      end
    end

    def callback(callback, key)
      if MatzBot::Commands.methods(false).include?(method = "#{callback}_#{key}")
        MatzBot::Commands.send(method)
      end
    end
  end
end
