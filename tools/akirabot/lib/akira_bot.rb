class AkiraBot
  VERSION = '1.0.0'

  attr_accessor :config, :socket, :last_nick

  def initialize config
    self.config = config
  end

  def start
    connect
    while true
      if IO.select([socket])
        react_to socket.gets
      end
    end
  end

  def connect
    c = config
    self.socket = TCPSocket.new(c[:server], c[:port])

    socket.puts "USER #{c[:user]} #{c[:nick]} #{c[:name]} :#{c[:name]} \r\n"
    socket.puts "NICK #{c[:nick]} \r\n"
    socket.puts "PRIVMSG NickServ :IDENTIFY #{c[:password]}" if c[:password]

    join c[:channel]
  end

  def reconnect
    socket.close
    self.socket = nil
    start
  end

  def react_to(line)
    warn line.chomp

    case line
    when /^\:(.+)\!\~?(.+)\@(.+) (PRIVMSG) \#?(\w+) \:(.+)/ then
      msg, nick = $6, $1
      warn [msg, nick].inspect
      execute(msg, nick)
    when /^PONG (.+)/ then
      socket.puts "PONG #{$1}"
    when /^ERROR/ then
      reconnect
    else
      # do nothing
    end
  end

  def execute(cmd, nick)
    self.last_nick = nick

    data = cmd.split

    return unless data.shift =~ /^#{config[:nick]}/i

    msg = "cmd_#{data.shift}"
    send msg, data if self.commands.include? msg
  end

  def join(channel, quit_prev = true)
    channel = "##{channel}" unless channel =~ /^#/
    socket.puts "PART #{config[:channel]}" if quit_prev
    socket.puts "JOIN #{channel} \r\n"
    config[:channel] = channel
  end

  def commands short = false
    c = methods.map { |m| m.to_s }.grep(/^cmd_/).sort
    c.map! { |m| m.sub(/^cmd_/, '') } if short
    c
  end

  def cmd_help(data)
    say("Commands I know: \0037 #{commands(:short).join(', ')}")
  end

  def say(data, channel = config[:channel])
    data = data.join(" ") if data.is_a?(Array)
    data.split("\n").each do |message|
      while message
        fragment, message = message[0..450], message[450..-1]
        socket.puts "PRIVMSG #{channel} :#{fragment}"
      end
      sleep 1
    end
    nil
  end

  def cmd_quit(data)
    socket.puts "QUIT :#{data.shift}"
    exit!
  end
end
