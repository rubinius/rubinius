require 'rubinius/bert'
require 'socket'
require 'readline'
require 'tempfile'

class Console
  def initialize(port, pid, path)
    @port = port.to_i
    @pid =  pid
    @path = path
  end

  def connect
    @socket = TCPSocket.new("localhost", @port)
    @decoder = BERT::Decode.new(@socket)
    @encoder = BERT::Encode.new(@socket)

    tuple = @decoder.read_any
    unless tuple[0] == :hello_query_agent
      puts "Error receiving hello: #{tuple.inspect}"
      return
    end

    puts "Connected to localhost:#{@port}, host type: #{tuple[1]}"
  end

  def take_commands
    while true
      str = Readline.readline("console> ")

      cmd, args = str.split(/\s+/, 2)

      case cmd
      when "uname", "info"
        uname(args)
      when "set"
        set_config(args)
      when "get"
        get_config(args)
      when "q", "quit", "exit"
        quit
        return
      when "bt", "backtrace"
        backtrace(args)
      when "gdb"
        start_gdb(args)
      else
        puts "Unknown command: #{cmd}"
      end
    end
  end

  def request(*args)
    @encoder.write_any t[*args]
    @decoder.read_any
  end

  def uname(args)
    response = request :uname

    os, hostname, os_version, full_version, arch = *response

    puts "      OS: #{os} #{os_version}"
    puts "Hostname: #{hostname}"
    puts " Machine: #{arch}"

    if args =~ /-v/
      puts "\n#{full_version}"
    end
  end

  def quit
    begin
      @encoder.write_any :close
      response = @decoder.read_any
    rescue Errno::EPIPE, IOError
      return
    end

    unless response == :bye
      puts "Unexpected response: #{response.inspect}"
    end
  end

  def set_config(args)
    var, val = args.split(/\s+/, 2)

    response = request :set_config, var, val

    case response
    when :ok
      puts "Set var '#{var}' ok."
    when :unknown_key
      puts "Unknown var '#{var}'."
    when :error
      puts "Error setting variable."
    end
  end

  def get_config(args)
    var = args.strip

    response = request :get_config, var

    if response == :unknown_key
      puts "Unknown variable '#{var}'."
    else
      ok, val = *response
      puts "var #{var} = #{val.inspect}"
    end
  end

  def backtrace(args)
    if args =~ /--vm/
      run_gdb "thread apply all bt"
    else
      response = request :backtrace

      if response[0] == :ok
        response[1].each_with_index do |str, idx|
          puts "Thread #{idx}:"
          puts str
          puts
        end
      end
    end
  end

  def run_gdb(str)
    Tempfile.open("rbx") do |tf|
      tf.puts str

      tf.close

      cmd = "gdb '#{@path}' '#{@pid}' --batch --quiet -x #{tf.path}"

      system cmd
    end
  end

  def start_gdb(args)
    if args.strip.empty?
      puts "Starting gdb for #{@pid}..."
      system "gdb '#{@path}' '#{@pid}'"
    else
      puts "Running '#{args}' under gdb:"
      run_gdb(args)
    end
  end

  def self.find_agents
    agents = Dir["#{ENV['TMPDIR']}/rubinius-agent.*"]

    if agents.size == 1
      pid, port, cmd, path = File.readlines(agents.first)
      return [pid.to_i, port.to_i, cmd.strip, path.strip]
    else
      return nil
    end
  end
end

if port = ARGV.shift
  port = port.to_i
  pid = nil
  cmd = nil
  path = nil

else
  pid, port, cmd, path = Console.find_agents
  unless pid
    puts "Unable to find any agents to connect to please specify one"
    exit 1
  end

  puts "VM: #{cmd}"

end

puts "Connecting to VM on port #{port}"

con = Console.new(port, pid, path)

con.connect

con.take_commands
