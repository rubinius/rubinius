# Contained first is the system startup code.
begin
  ENV = Rubinius::EnvironmentVariables.new

  String.ruby_parser if ENV['RUBY_PARSER']
  String.sydney_parser if ENV['SYDNEY'] or ENV['SYDPARSE']

  # define a global "start time" to use for process calculation
  $STARTUP_TIME = Time.now
rescue Object => e
  STDOUT << "Error detected running loader startup stage:\n"
  STDOUT << "  #{e.message} (#{e.class})\n"
  STDOUT << e.backtrace
  exit 2
end

# Set up a handler for SIGINT that raises Interrupt on the main thread
Signal.trap("INT") do |sig|
  raise Interrupt, "Thread has been interrupted"
end

# This is the end of the kernel and the beginning of specified
# code. We read out of ARGV to figure out what the user is
# trying to do.

# Setup $LOAD_PATH.

# Add a fallback directory if Rubinius::LIB_PATH doesn't exist
main_lib = File.expand_path(Rubinius::LIB_PATH)
main_lib = File.join(Dir.pwd, 'lib') unless File.exists?(main_lib)

# This conforms more closely to MRI. It is necessary to support
# paths that mkmf adds when compiling and installing native exts.
additions = []
additions << Rubinius::SITELIBDIR
additions << Rubinius::SITEARCHDIR
additions << Rubinius::SITEDIR
additions << Rubinius::RUBYLIBDIR
additions << main_lib
additions.uniq!

# The main stdlib location
# HACK todo remove this comment when we're setting this constant in the VM
# additions << Rubinius::CODE_PATH

$LOAD_PATH.unshift(*additions)

if ENV['RUBYLIB'] and not ENV['RUBYLIB'].empty? then
  rubylib_paths = ENV['RUBYLIB'].split(':')
  $LOAD_PATH.unshift(*rubylib_paths)
end

# Allow system wide code preloading

['/etc/rbxrc',"#{ENV['HOME']}/.rbxrc",ENV['RBX_PRELOAD']].each do |file|
  begin
    load file if file and File.exist?(file)
  rescue LoadError
    nil
  end
end

# Parse options here!
RBS_USAGE = <<END
Usage: rubinius [options] [file]
  File may be any valid Ruby source file (.rb) or a compiled Ruby file (.rbc).

Options:
  -d             Enable debugging output and set $DEBUG to true.
  -dc            Display debugging information for the compiler.
  -dl            Display debugging information for the loader.
  -debug         Launch the debugger.
  -remote-debug  Run the program under the control of a remote debugger.
  -e 'code'      Directly compile and execute code (no file provided).
  -Idir1[:dir2]  Add directories to $LOAD_PATH.
  -S script      Run script using PATH environment variable to find it.
  -P             Run the profiler.
  -Pcolumn       Run the profiler and sort output by column.
  -Ps            Run the Selector profiler.
  -Pss           Run the SendSite profiler.
  -rlibrary      Require library before execution.
  --sydney       Use SydneyParser.
  --ruby_parser  Use RubyParser.
  -w             Enable warnings. (currently does nothing--compatibility)
  -v             Display the version and set $VERBOSE to true.
END

$VERBOSE = false
code = 0

show_selectors = false
show_sendsites = false

TOPLEVEL_BINDING = binding()

show_eval = false
eval_code = nil
script = nil

width = 80
if Rubinius::Terminal and !ENV['RBX_NO_COLS']
  begin
    `which tput &> /dev/null`
    if $?.exitstatus == 0
      width = `tput cols`.to_i
    end
  end
end

Rubinius.const_set 'TERMINAL_WIDTH', width

begin

  script_debug_requested = false
  until ARGV.empty?
    arg = ARGV.shift
    case arg
    when '--'
      break
    when '-h', '--help'
      puts RBS_USAGE
      exit 1
    when "-v"
      puts "rubinius #{Rubinius::RBX_VERSION} (ruby #{RUBY_VERSION}) (#{Rubinius::BUILDREV[0..8]} #{RUBY_RELEASE_DATE}) [#{RUBY_PLATFORM}]"
      $VERBOSE = true
      exit 0 if ARGV.empty?
    when "-vv"
      puts "rubinius #{Rubinius::RBX_VERSION} (ruby #{RUBY_VERSION}) (#{Rubinius::BUILDREV[0..8]} #{RUBY_RELEASE_DATE}) [#{RUBY_PLATFORM}]"
      $VERBOSE = true
      puts "Options:"
      puts "  Interpreter type: #{Rubinius::INTERPRETER}"
      if jit = Rubinius::JIT
        puts "  JIT enabled: #{jit}"
      else
        puts "  JIT disabled"
      end
      puts
      exit 0 if ARGV.empty?
    when "-w"
      # do nothing (HACK)
    when '-dc'
      puts "[Compiler debugging enabled]"
      $DEBUG_COMPILER = true
    when '-dl'
      $DEBUG_LOADING = true
      puts "[Code loading debugging enabled]"
    when '-d'
      $DEBUG = true
    when '-debug'
      require 'debugger/interface'
      Debugger::CmdLineInterface.new
      script_debug_requested = true
    when '-remote-debug'
      require 'debugger/debug_server'
      if port = (ARGV.first =~ /^\d+$/ and ARGV.shift)
        $DEBUG_SERVER = Debugger::Server.new(port.to_i)
      else
        $DEBUG_SERVER = Debugger::Server.new
      end
      $DEBUG_SERVER.listen
      script_debug_requested = true
    when '-Ps'
      count = (ARGV.first =~ /^\d+$/) ? ARGV.shift : '30'
      show_selectors = count.to_i
    when '-Pss'
      count = (ARGV.first =~ /^\d+$/) ? ARGV.shift : '30'
      show_sendsites = count.to_i
    when '-P'
      require 'profile'
    when /P\w/
      require 'profile'
      Profiler__.options :sort => arg[2..-1].to_sym
    when '-gc'
      stats = Rubinius::Stats::GC.new
      at_exit { stats.show }
    when '-S'
      script = ARGV.shift
      sep    = File::PATH_SEPARATOR
      search = ENV['PATH'].split(sep).unshift(Rubinius::BIN_PATH)
      file   = search.detect do |d|
        path = File.join(d, script)
        File.exist?(path) ? path : nil
      end

      $0 = script if file

      # if missing, let it die a natural death
      ARGV.unshift file ? file : script
    when '--sydney'
      String.sydney_parser
    when '--ruby_parser'
      String.ruby_parser
    when '-e'
      $0 = "(eval)"
      eval_code = ARGV.shift
    when '-ed'
      show_eval = true
    else
      if arg.prefix? "-I"
        more = arg[2..-1]
        if more.empty?
          path = File.expand_path ARGV.shift
          $LOAD_PATH.unshift path
        else
          more.split(":").reverse_each do |path|
            path = File.expand_path path
            $LOAD_PATH.unshift(path)
          end
        end
      elsif arg.prefix? "-r"
        # TODO: parse all the args before requiring any files
        # because $0 needs to be set before files are required
        # e.g. 'rbx -r mkmf extconf.rb', mkmf expects to use $0
        more = arg[2..-1]
        if more.empty?
          require ARGV.shift
        else
          require more
        end
      elsif arg.prefix? "-i"
        # in place edit mode
        $-i = arg[2..-1]
      elsif arg == "-"
        $0 = "-"
        Compiler::Utils.execute STDIN.read
      elsif arg.prefix? "-"
        puts "Invalid switch '#{arg}'"
        puts RBS_USAGE
        exit! 1
      else
        script = arg
        # And we're done.
        break
      end
    end
  end

  # If someone used -e, run that code.
  if eval_code
    # If we also caught a script to run, we just treat it like
    # another arg.
    ARGV.unshift script if script
    eval(eval_code, TOPLEVEL_BINDING) do |compiled_method|
      if show_eval
        p eval_code.to_sexp("(eval)", 1)
        puts compiled_method.decode
      end
    end
  elsif script
    if File.exist?(script)
      $0 = script
      Compiler::Utils.debug_script! if script_debug_requested
      Compiler::Utils.load_from_extension arg
    else
      if script.suffix?(".rb")
        puts "Unable to find '#{script}'"
        exit! 1
      else
        prog = File.join main_lib, "bin", "#{script}.rb"
        if File.exist? prog
          $0 = prog
          load prog
        else
          raise LoadError, "Unable to find a script '#{script}' to run"
        end
      end
    end
  end

  unless $0
    if Rubinius::Terminal
      repr = ENV['RBX_REPR'] || "bin/irb"
      $0 = repr
      prog = File.join main_lib, repr
      begin
        # HACK: this was load but load raises LoadError
        # with prog == "lib/bin/irb". However, require works.
        # Investigate when we have specs running.
        require prog
      rescue LoadError => e
        STDERR.puts "Unable to find repr named '#{repr}' to load."
        puts e.awesome_backtrace.show
        exit 1
      end
    else
      $0 = "(eval)"
      Compiler::Utils.execute "p #{STDIN.read}"
    end
  end

rescue SystemExit => e
  code = e.status


rescue SyntaxError => e
  puts "A syntax error has occured:"
  puts "    #{e.message}"
  puts "    near line #{e.file}:#{e.line}, column #{e.column}"
  puts "\nCode:\n#{e.code}"
  if e.column
    puts((" " * (e.column - 1)) + "^")
  end

  puts "\nBacktrace:"
  puts e.awesome_backtrace.show
  code = 1

rescue Object => e
  begin
    if e.kind_of? Exception # or e.kind_of? ThrownValue
      msg = e.message
    else
      msg = "strange object detected as exception: #{e.inspect}"
    end

    puts "An exception has occurred:"
    puts "    #{e.message} (#{e.class})"

    puts "\nBacktrace:"
    puts e.awesome_backtrace.show
    code = 1

  rescue Object => e2
    puts "\n====================================="
    puts "Exception occurred during top-level exception output! (THIS IS BAD)"
    puts
    puts "Original Exception: #{e.inspect} (#{e.class})"
    puts "New Exception: #{e2.inspect} (#{e.class})"
    code = 128
  end
end

begin
  Rubinius::AtExit.shift.call until Rubinius::AtExit.empty?
rescue SystemExit => e
  code = e.status
rescue Object => e
  puts "An exception occurred inside an at_exit handler:"
  puts "    #{e.message} (#{e.class})"
  puts "\nBacktrace:"
  puts e.awesome_backtrace.show
  code = 1
end

begin
  ObjectSpace.run_finalizers
rescue Object => e
  puts "An exception occured while running object finalizers:"
  puts "    #{e.message} (#{e.class})"
  puts "\nBacktrace:"
  puts e.awesome_backtrace.show
  code = 1
end

if show_selectors
  ps = Rubinius::Profiler::Selectors.new
  begin
    ps.show_stats show_selectors
  rescue Object => e
    puts "An exception occured while running selector profiler:"
    puts "    #{e.message} (#{e.class})"
    puts "\nBacktrace:"
    puts e.awesome_backtrace.show
    code = 1
  end
end

if show_sendsites
  ps = Rubinius::Profiler::SendSites.new
  begin
    ps.show_stats show_sendsites
  rescue Object => e
    puts "An exception occured while running sendsite profiler:"
    puts "    #{e.message} (#{e.class})"
    puts "\nBacktrace:"
    puts e.awesome_backtrace.show
    code = 1
  end
end

if Rubinius::RUBY_CONFIG['rbx.jit_stats']
  stats = Rubinius::VM.jit_info
  puts "JIT time spent: #{stats[0] / 1000000}ms"
  puts " JITed methods: #{stats[1]}"
end

if Rubinius::RUBY_CONFIG['rbx.gc_stats']
  Rubinius::Stats::GC.new.show
end

Process.exit(code || 0)
