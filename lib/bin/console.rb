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

  def connect(notice=true)
    @socket = TCPSocket.new("localhost", @port)
    @decoder = BERT::Decode.new(@socket)
    @encoder = BERT::Encode.new(@socket)

    tuple = @decoder.read_any
    unless tuple[0] == :hello_query_agent
      puts "Error receiving hello: #{tuple.inspect}"
      return
    end

    if notice
      puts "Connected to localhost:#{@port}, host type: #{tuple[1]}"
    end
  end

  def take_commands
    while true
      str = Readline.readline("console> ")

      cmd, args = str.split(/\s+/, 2)

      case cmd
      when "set"
        set_config(args)
      when "get"
        get_config(args)
      when "memory", "mem"
        show_memory
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
  rescue Errno::EPIPE, IOError
    puts "VM has disconnected"
  end

  def request(*args)
    @encoder.write_any t[*args]
    @decoder.read_any
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

    begin
      kind, val = get(var)
    rescue GetError => e
      puts "Error: #{e.message}"
      return
    end

    if val.kind_of? Array
      puts "var #{var} = ["
      val.each do |x|
        puts "  #{x.inspect},"
      end
      puts "]"
    else
      puts "var #{var} = #{val.inspect}"
    end
  end

  class GetError < RuntimeError
    def initialize(var, code, msg)
      super("#{code} - #{msg} (#{var})")
    end
  end

  def get(var)
    @encoder.write_any t[:get_config, var]
    code, kind, val = @decoder.read_any

    if code != :ok
      raise GetError.new(var, code, kind)
    end

    return [kind, val]
  end

  def human_number(number)
    k = number >> 10
    if k > 1024
      "%.2fMb" % (number.to_f / (1024 * 1024))
    elsif k == 0
      "#{number}b"
    else
      "#{k}Kb"
    end
  end

  def show_memory
    code, val = get "system.memory.young"
    puts "  Young: #{val}"

    total = val

    code, val = get "system.memory.mature"
    puts " Mature: #{val}"

    total += val

    code, val = get "system.memory.large"
    puts "  Large: #{val}"

    total += val

    code, val = get "system.memory.code"
    puts "   Code: #{val}"

    total += val

    code, val = get "system.memory.symbols"
    puts "Symbols: #{val}"

    total += val

    puts " Total: #{human_number(total)} (#{total})"
  end

  def backtrace(args)
    if args =~ /--vm/
      run_gdb "thread apply all bt"
    else
      kind, val = get "system.backtrace"

      val.each_with_index do |str, idx|
        puts "Thread #{idx}:"
        puts str
        puts
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

if false # port = ARGV.shift
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

end

if ARGV.empty?
  con = Console.new(port, pid, path)

  puts "VM: #{cmd}"
  puts "Connecting to VM on port #{port}"

  con.connect
  con.take_commands
else
  con = Console.new(port, pid, path)
  con.connect(false)
  con.get_config(ARGV[0])
end
