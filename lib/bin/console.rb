require 'rubinius/agent'
require 'socket'
require 'readline'
require 'tempfile'
require 'optparse'
require 'ostruct'

class Console
  def initialize(port, pid, path)
    @port = port.to_i
    @pid =  pid
    @path = path
  end

  def connect(notice=true)
    @agent = Rubinius::Agent.connect "localhost", @port

    if notice
      puts "Connected to localhost:#{@port}, host type: #{@agent.handshake[1]}"
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
      when "pid"
        show_pid
      when "q", "quit", "exit"
        quit
        return
      when "bt", "backtrace"
        backtrace(args)
      when "gdb"
        start_gdb(args)
      when "help"
        help
      else
        if cmd
          if !args or args.empty?
            get_config(cmd)
          else
            puts "Unknown command: #{cmd}"
          end
        end
      end
    end
  rescue Errno::EPIPE, IOError
    puts "VM has disconnected"
  end

  def help
    puts <<-STR
set <var> <value> - Set a variable
get <var>         - Get a variable
backtrace         - Show backtraces of all Threads
gdb               - Connect to process via gdb
pid               - Show process ID
memory, mem       - Show memory statistics
help              - You're lookin' at it
    STR
  end

  def quit
    response = @agent.close

    unless response == :bye
      puts "Unexpected response: #{response.inspect}"
    end
  end

  def set_config(args)
    var, val = args.split(/\s+/, 2)

    response = @agent.request :set_config, var, val

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
    rescue Rubinius::Agent::GetError => e
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

  def get(var)
    @agent.get(var)
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

  def show_pid
    code, val = get "system.pid"
    puts "PID: #{val}"
  end

  def show_memory
    code, val = get "system.memory.young.bytes"
    puts "  Young: #{val}"

    total = val

    code, val = get "system.memory.mature.bytes"
    puts " Mature: #{val}"

    total += val

    code, val = get "system.memory.large.bytes"
    puts "  Large: #{val}"

    total += val

    code, val = get "system.memory.code.bytes"
    puts "   Code: #{val}"

    total += val

    code, val = get "system.memory.symbols.bytes"
    puts "Symbols: #{val}"

    total += val

    puts "  Total: #{human_number(total)} (#{total})"
  end

  def backtrace(args="")
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

  class Agent
    def initialize(pid, port, cmd, path)
      @pid = pid
      @port = port
      @command = cmd
      @path = path
    end

    attr_reader :pid, :port, :command, :path

    def self.find_all
      agents = Dir["#{ENV['TMPDIR']}/rubinius-agent.*"]

      return [] unless agents

      agents.map do |path|
        pid, port, cmd, exec = File.readlines(path)
        Agent.new(pid.to_i, port.to_i, cmd.strip, exec.strip)
      end
    end
  end
end

options = OpenStruct.new

opt = OptionParser.new do |o|
  o.on "--ps" do
    agents = Console::Agent.find_all
    puts "Agents found: #{agents.size}"
    agents.each do |agent|
      puts "#{agent.pid} - #{agent.command} (port:#{agent.port})"
    end
    exit 0
  end

  o.on "-v", "--vm NUM" do |val|
    options.vm = val.to_i
  end

  o.on "-p", "--port NUM" do |val|
    options.port = val.to_i
  end

  o.on "-g", "--get NAME" do |val|
    options.get = val
    options.quiet = true
  end

  o.on "--bt", "--backtrace" do
    options.backtrace = true
    options.quiet = true
  end
end

opt.parse! ARGV

if options.port
  unless options.quiet
    puts "Connecting to port #{options.port}"
  end
  con = Console.new(options.port, nil, nil)
else
  agents = Console::Agent.find_all

  if options.vm
    agent = agents.detect { |a| a.pid == options.vm }
    unless agent
      puts "Unable to find agent '#{options.vm}'"
      exit 1
    end
  else
    case agents.size
    when 0
      puts "Unable to find any agents to connect to please specify one"
      exit 1
    when 1
      agent = agents.first
    else
      puts "Please select which agent:"
      i = 1
      agents.each do |a|
        puts "#{i}: #{a.pid} - #{a.command}"
        i += 1
      end

      which = Readline.readline.strip.to_i

      unless agent = agents[which - 1]
        puts "Invalid choise"
        exit 1
      end
    end
  end

  con = Console.new(agent.port, agent.pid, agent.path)

  unless options.quiet
    puts "VM: #{agent.command}"
    puts "Connecting to VM on port #{agent.port}"
  end
end

if options.get
  con.connect(false)
  kind, var = con.get(options.get)
  case var
  when String
    puts var
  when Integer
    puts var.to_i
  when BERT::Tuple
    print "[\n"
    puts var.map { |v| "  #{v}" }.join(",\n")
    puts "]"
  else
    p var
  end
elsif options.backtrace
  con.connect(false)
  con.backtrace
else
  con.connect
  con.take_commands
end
