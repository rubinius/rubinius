require 'socket'

module MatzBot
  module Client
    extend self

    attr_accessor :config, :socket, :last_nick, :authorized

    def start(options)
      self.config ||= {}
      self.config.merge! Hash[*options.map { |k,v| [k.intern, v] }.flatten]

      connect!
      main_loop
    end

    def connect!
      log "Connecting to #{config[:server]}:#{config[:port]}..."
      self.socket = TCPSocket.new(config[:server], config[:port])

      socket.puts "USER #{config[:user]} #{config[:nick]} #{config[:name]} :#{config[:name]} \r\n"
      socket.puts "NICK #{config[:nick]} \r\n"

      socket.puts "PRIVMSG NickServ :IDENTIFY #{config[:password]}" if config[:password]

      # channel might not have a # in front of it, so add it
      config[:channel] = config[:channel][/^#/] ? config[:channel] : '#' + config[:channel]
      join config[:channel]
    end

    def reconnect!
      socket.close
      self.socket = nil
      start
    end
    
    def main_loop
      while true
        if IO.select([socket])
          react_to socket.gets
        end
      end
      #socket.each do |line|      
      #  react_to line  # its own method so we can change it on the fly, without hurting the loop
      #end
    end

    def react_to(line)
      begin
        MatzBot.reload!
      rescue Exception => bang 
        say "Class load error #{bang.class} (#{caller[1]}), #{bang}."
      end      

      self.authorized = false # not authorized

      info = grab_info(line) # grabs the info from an PRIVMSG
      puts line              # puts to the console

      pong(line) if line[0..3] == "PING" # keep-alive

      if info && info.last    # only called if grabbing the info was successful
        log_message info    # logs in a friendly format, in chat.txt
        execute(info.last, info.first) if info
      elsif has_error?(line)
        reconnect! 
      end
    end

    def has_error?(line)
      log "Error from server: #{line}" and return true if line[/^ERROR/]
    end
    
    def execute(cmd, nick)
      data = cmd.split
      return false unless data && data.first && authorize?(data)

      self.last_nick = nick

      data.join(' ').split(' then ').each do |command|
        # callbacks
        filters(:listen).each do |filter|
          filter.call(command) if command
        end if filters(:listen).size.nonzero?

        command = command.split(' ')
        command.shift if command.first =~ /^#{config[:nick]}/i

        if Commands.methods.include? command.first and !(EmptyModule.methods.include? command.first)
          Commands.send(command.first, command[1..-1])      
        #else
        #  say "no command #{command}"
        end
      end
    rescue Exception => bang
      say "Command error #{bang.class}, #{bang}."
      say " #{bang.backtrace.first}"
    end

    def say(message)
      Commands.say message
    end

    def filters(type)
      Commands.send(:filters, type)
    end
    
    def pong(line)
      line[0..3] = "PONG"
      socket.puts "#{line}"
      puts "#{line}"
      Commands.poll(line) if Commands.methods.include? 'poll'
    end
    
    def grab_info(text)
      # The following is the format of what the bot recieves:
      # :kyle!~kyle@X-24735511.lmdaca.adelphia.net PRIVMSG #youngcoders :for the most part
      # :nick!~ident@host PRIVMSG #channel :message
      text =~ /^\:(.+)\!\~?(.+)\@(.+) PRIVMSG \#?(\w+) \:(.+)/ ? [$1, $2, $3, $4, $5] : false
    end
    
    def authorize?(data)
      if self.config[:only_when_addressed] and data.first != "#{self.config[:nick]}:"
        return false
      end
      
      command, password = data.first, data[1]
      if Commands.protected_instance_methods(false).include? command
        self.authorized = config[:password] && password == config[:password]
        data.delete_at(1) 
        authorized
      else true
      end
    end
    
    def join(channel, quit_prev = true)
      socket.puts "PART #{config[:channel]}" if quit_prev
      socket.puts "JOIN #{channel} \r\n"
      config[:channel] = channel
    end
    
    def log_message(array)
      log "<#{array[0]}> : #{array[4]}"
    end

    def log(string)
      File.open("chat.txt", "a") do |f|
        f.puts "[#{Time.new.strftime "%m/%d/%Y %I:%M %p PST"}] #{string} \n"
      end
    end
  end
end

module EmptyModule
end

