require 'set'
require 'yaml'

module MatzBot
  module Commands
    extend self

    def help(data)
      config[:hidden_methods] ||= []
      hidden = config[:hidden_methods].map { |m| m.to_s }
      if is_admin?
        commands = protected_instance_methods(false) - hidden
        command  = :pm
      else 
        commands = public_instance_methods(false) - hidden
        command  = :say
      end

      send(command, "Commands I know: \0037 #{commands.sort * ', '}")
    end

    def action(data)
      data = [data].flatten
      socket.puts "PRIVMSG #{config[:channel]} :\01ACTION #{data * ' '}\01"
      sleep 1
    end

    def say(data, channel = config[:channel])
      data = data.join(" ") if data.is_a?(Array)
      data.split("\n").each do |message|
        message = filters(:say).inject(message) do |string, filter|
          filter.call(string)
        end if filters(:say).size.nonzero?
        while message 
          fragment, message = message[0..450], message[450..-1]
          socket.puts "PRIVMSG #{channel} :#{fragment}"          
        end
        sleep 1
      end
      nil
    end
    alias :puts :say
    private :puts

    def save!(data)
      Session.save
      puts "Saved session data. ;)"
    end

  protected
    def quit(data)
      socket.puts "QUIT :#{data.shift}"
      exit
    end
    
    def update_nick(data)
      socket.puts "NICK :#{nick = data.shift}"
      config[:nick] = nick
    end

 private
    def session
      Session.session
    end
 
    def reply(string)
      say "#{Client.last_nick}: #{string}" if Client.last_nick
    end

    def pm(data, nick = Client.last_nick)
      say(data, nick)
    end

    # filter :say => :drunk_say
    # filter :listen => :google_translate
    def filter(type_and_methods)
      type = type_and_methods.keys.first
      Array(type_and_methods[type]).each do |method|
        filters(type) <<
          case method
          when Symbol, String then proc { |message| send(method, message) }     
          when Proc then method
          end
      end
    end

    def filters(type)
      $filters ||= {}
      $filters[type.to_sym] ||= Set.new
    end

    def reload_filters!
      $filters.clear if $filters
    end

    def help_method(options = {})
      # help_method :svn => [ :add_repo, :clear_repos ]
      options.each do |method, wraps|
        define_method(method) do |*data|
          wraps.map! { |m| m.to_s } 
          command = :pm
          unless is_admin?
            wraps -= protected_instance_methods(false)
            command = :say
          end
          send(command, "Commands for `#{method}': #{wraps.sort * ', '}")
        end
        config[:hidden_methods] ||= Set.new
        config[:hidden_methods] += wraps.map { |m| m.to_s }
      end
    end

    def socket
      Client.socket
    end

    def config
      Client.config ||= {}
    end

    def is_admin?
      Client.authorized
    end

    def needs_gem(hash)
      # needs_gem 'hpricot' => [ :method1, :method2 ]
      config[:failed_methods] ||= Set.new
      gem = ''
      hash.keys.each { |gem| require gem }
    rescue LoadError
      config[:failed_methods] += Array(hash[gem])
    end

    def method_added(method)
      config[:failed_methods] ||= Set.new
      remove_method method if config[:failed_methods].include?(method)
    end
  end
end

