# Contained first is the system startup code.

# Re-setup all the stdio channels, to pull in new ivars

STDOUT.setup
STDIN.setup
STDERR.setup

begin
  Array.after_loaded
  Module.after_loaded

  Class.after_loaded
  Hash.after_loaded
  Kernel.after_loaded
  Math.after_loaded
  FileTest.after_loaded
  Thread.after_loaded
  Struct.after_loaded
  Signal.after_loaded
  ObjectSpace.after_loaded
  IO.after_loaded
  Object.after_loaded

  ENV = EnvironmentVariables.new

  # define a global "start time" to use for process calculation
  $STARTUP_TIME = Time.now
rescue Object => e
  STDOUT << "Error detected running loader startup stage:\n"
  STDOUT << "  #{e.message} (#{e.class})\n"
  STDOUT << e.backtrace
  exit 2
end

# This is the end of the kernel and the beginning of specified
# code. We read out of ARGV to figure out what the user is
# trying to do.

Signal.action("INT") do |thr|  
  # We can't raise on ourselves, we raise on main.
  if thr == Thread.current
    thr = Thread.main
  end

  # The current thread might be dead if all other live threads are
  # a sleep when the current one died.
  unless thr.alive?

    thr = Thread.main
    # If main is dead too. Wow. Ok.. well... tell the user.
    unless thr.alive?
      puts "Signal received, but the main thread is dead."
      puts "Unable to continue."
      exit! 1
    end
  end

  # Push the output down a little bit, makes things look more
  # obvious that the system was interrupted.
  puts
  thr.raise Interrupt, "Thread has been interrupted"
end

# Setup $LOAD_PATH.

# If there is a prefix set, then adjust lib and stdlib to be
# relative to it.
if ENV['RBX_PREFIX']
  ["lib", "stdlib"].each do |lib|
    idx = $LOAD_PATH.index(lib)
    $LOAD_PATH[idx] = File.join ENV['RBX_PREFIX'], lib
  end
end

additions = []

# The main stdlib location
additions << Rubinius::CODE_PATH

$LOAD_PATH.insert($LOAD_PATH.index('.'), *additions)

if ENV['RUBYLIB'] and not ENV['RUBYLIB'].empty? then
  $LOAD_PATH.unshift(*ENV['RUBYLIB'].split(':'))
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
  -e 'code'      Directly compile and execute code (no file provided).
  -Idir1[:dir2]  Add directories to $LOAD_PATH.
  -p             Run the profiler.
  -rlibrary      Require library before execution.
  -w             Enable warnings. (currently does nothing--compatibility)
  -v             Display the version and set $VERBOSE to true.

END

$VERBOSE = false
code = 0

# Setup the proper staticscope
MethodContext.current.method.staticscope = StaticScope.new(Object)

TOPLEVEL_BINDING = binding()

begin

  until ARGV.empty?
    arg = ARGV.shift
    case arg
    when '--'
      break
    when '-h'
      puts RBS_USAGE
      exit 1
    when "-v"
      puts "rubinius #{Rubinius::RBX_VERSION} (ruby #{Rubinius::RUBY_VERSION} compatible) (#{Rubinius::BUILDREV[0..8]}) (#{RUBY_RELEASE_DATE}) [#{RUBY_PLATFORM}]"
      $VERBOSE = true
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
    when '-p'
      require 'profile'
    when '-e'
      $0 = "(eval)"
      Compile.execute ARGV.shift
    else
      if arg.prefix? "-I"
        more = arg[2..-1]
        if more.empty?
          $LOAD_PATH.unshift(ARGV.shift)
        else
          more.split(":").reverse_each do |path|
            $LOAD_PATH.unshift(path)
          end
        end
      elsif arg.prefix? "-r"
        more = arg[2..-1]
        if more.empty?
          require ARGV.shift
        else
          require more
        end
      elsif arg == "-"
        $0 = "-"
        Compile.execute STDIN.read
      elsif arg.prefix? "-"
        puts "Invalid switch '#{arg}'"
        exit! 1
      else
        if File.exists?(arg)
          $0 = arg
          Compile.load_from_extension arg 
        else
          if arg.suffix?(".rb")
            puts "Unable to find '#{arg}'"
            exit! 1
          else
            prog = "bin/#{arg}"
            begin
              $0 = prog
              require prog
            rescue LoadError => e
              puts "Unable to find program '#{arg}' ('#{prog}'): #{e.message} (#{e.class})"
              exit! 1
            end
          end
        end
        break
      end
    end
  end
  
  unless $0
    if Rubinius::Terminal
      ran = false
      unless /^(rubinius|rbx)$/.match(ARG0)
        begin
          $0 = ARG0
          require "bin/#{ARG0}"
          ran = true
        rescue LoadError
        end
      end

      unless ran
        repr = ENV['RBX_REPR'] || "bin/irb"
        $0 = repr
        require repr
      end
    else
      $0 = "(eval)"
      Compile.execute "p #{STDIN.read}"
    end
  end
  
rescue SystemExit => e
  code = e.code
rescue Object => e
  begin
    if e.kind_of? Exception or e.kind_of? ThrownValue
      msg = e.message
    else
      msg = "strange object detected as exception: #{e.inspect}"
    end
    if e.kind_of? SyntaxError
      puts "A syntax error has occured:"
      puts "    #{msg}"
      puts "    near line #{e.file}:#{e.line}, column #{e.column}"
      puts "\nCode:\n#{e.code}"
      if e.column
        puts((" " * (e.column - 1)) + "^")
      end
    else
      puts "An exception has occurred:"
      puts "    #{msg} (#{e.class})"
    end
    puts "\nBacktrace:"
    puts e.awesome_backtrace.show
    code = 1
  rescue Object => e2
    puts "Unable to build backtrace due to errors"
    puts "Original Exception: #{e.inspect} (#{e.class})"
    puts "New Exception: #{e2.inspect} (#{e.class})"
    code = 128
  end
end

begin
  Rubinius::AtExit.shift.call until Rubinius::AtExit.empty?
rescue SystemExit => e
  code = e.code
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

Process.exit(code || 0)

