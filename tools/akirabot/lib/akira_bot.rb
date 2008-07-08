require 'socket'

class AkiraBot
  VERSION = '1.0.0'

  DEFAULTS = {
    :channel => nil,
    :name => 'Akira Bot',
    :nick => 'akirabot',
    :password => nil,
    :port => 6667,
    :server => 'irc.freenode.org',
    :user => 'akirabot',
  }

  attr_accessor :config, :socket, :last_nick, :quit

  def cmd_help(data)
    say("Commands I know: \0037 #{commands.join(', ')}")
  end

  def cmd_quit(data)
    puts "QUIT :#{data.shift}"
    self.quit = true
  end

  def commands
    methods.map { |m| m.to_s }.grep(/^cmd_/).sort.map { |m| m.sub(/^cmd_/, '') }
  end

  def connect
    c = config
    self.socket ||= TCPSocket.new(c[:server], c[:port])

    puts "USER #{c[:user]} #{c[:nick]} #{c[:name]} :#{c[:name]}"
    puts "NICK #{c[:nick]}"
    say "IDENTIFY #{c[:password]}", "NickServ" if c[:password]

    join c[:channel] if c[:channel]
  end

  def execute(msg, nick)
    cmd = msg.split
    who = cmd.shift
    msg = cmd.shift

    return unless who =~ /^#{config[:nick]}/i && self.commands.include?(msg)

    self.last_nick = nick
    send "cmd_#{msg}", cmd
  end

  def initialize config = {}
    self.quit = false
    self.config = DEFAULTS.merge(config)
  end

  def join(channel, quit_prev = true)
    channel = "##{channel}" unless channel =~ /^#/
    puts "PART #{config[:channel]}" if quit_prev
    puts "JOIN #{channel}"
    config[:channel] = channel
  end

  def puts str
    socket.puts "#{str}\r\n"
  end

  def react_to(line)
    warn line.chomp

    case line
    when /^\:(.+)\!\~?.+\@.+ PRIVMSG \#?\w+ \:(.+)/ then
      nick, msg = $1, $2
      execute(msg, nick)
    when /^PONG (.+)/ then
      puts "PONG #{$1}"
    when /^ERROR/ then
      reconnect
    else
      # do nothing
    end
  end

  def reconnect
    socket.close
    self.socket = nil
    connect
  end

  def say(data, channel = config[:channel])
    data = data.join(" ") if Array === data

    data.split("\n").each do |message|
      while message
        fragment, message = message[0..450], message[450..-1]
        puts "PRIVMSG #{channel} :#{fragment}"
        sleep 0.5 if message
      end
    end

    nil
  end

  def start
    connect
    loop do
      react_to(socket.gets) if IO.select([socket])
      break if self.quit
    end
  end
end
