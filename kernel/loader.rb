# Contained first is the system startup code.

begin
  Array.after_loaded
  Module.after_loaded
  Readline.after_loaded

  Class.after_loaded
  Hash.after_loaded
  Kernel.after_loaded
  Actor.after_loaded
  Math.after_loaded
  FileTest.after_loaded

  ENV = EnvironmentVariables.new

  # define a global "start time" to use for process calculation
  $STARTUP_TIME = Time.now
rescue Object => e
  puts "Error detected running loader startup stage:"
  puts "  #{e.message} (#{e.class})"
  exit 2
end

# This is the end of the kernel and the beginning of specified
# code. We read out of ARGV to figure out what the user is
# trying to do.

Signal.trap("INT") do
  # Push the output down a little bit, makes things look more
  # obvious that the system was interrupted.
  puts
  Thread.main.raise Interrupt, "Thread has been interrupted"
end

# Setup $LOAD_PATH.

additions = []

# If there is no compiler.rba or COMPILER env variable, use the system one.
if ENV["COMPILER"] and File.exists? ENV["COMPILER"]
  additions << ENV["COMPILER"]
end

additions << File.join(Rubinius::RBA_PATH, "compiler")
additions << File.join(Rubinius::RBA_PATH, "compiler.rba")

# The main stdlib location
additions << Rubinius::CODE_PATH

$LOAD_PATH.insert($LOAD_PATH.index('.'), *additions)

if ENV['RUBYLIB'] and not ENV['RUBYLIB'].empty? then
  $LOAD_PATH.unshift(*ENV['RUBYLIB'].split(':'))
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
  -v             Display the version and set $VERBOSE to true.

END

# script = ARGV.shift

if ARGV.include?('-p')
  ARGV.delete '-p'
  require 'profile'
end
$VERBOSE = false
code = 0
ran_something = false

begin

  until ARGV.empty?
    arg = ARGV.shift
    case arg
    when '-h'
      puts RBS_USAGE
      exit 1
    when "-v"
      puts "rubinius #{Rubinius::RBX_VERSION} (ruby #{Rubinius::RUBY_VERSION} compatible) (#{Rubinius::BUILDREV[0..8]}) (#{RUBY_RELEASE_DATE}) [#{RUBY_PLATFORM}]"
      $VERBOSE = true
    when '-dc'
      puts "[Compiler debugging enabled]"
      $DEBUG_COMPILER = true
    when '-dl'
      $DEBUG_LOADING = true
      puts "[Code loading debugging enabled]"
    when '-d'
      $DEBUG = true
    when '-c'
      puts "Deprecated. Use 'rbx compile' instead."
      file = ARGV.shift
      path = compile(file)
      puts "Compiled #{file} to #{path}"
      exit 1
    when '-e'
      Compile.execute ARGV.shift
      ran_something = true
    else
      if arg.prefix? "-I"
        more = arg[2..-1]
        if more.empty?
          $:.unshift(ARGV.shift)
        else
          more.split(":").each do |path|
            $:.unshift(path)
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
        ran_something = true
        Compile.execute STDIN.read
      elsif arg.prefix? "-"
        puts "Invalid switch '#{arg}'"
        exit! 1
      else
        if File.exists?(arg)
          ran_something = true
          load(arg)
        else
          if arg.suffix?(".rb")
            puts "Unable to find '#{arg}'"
            exit! 1
          else
            prog = "bin/#{arg}"
            begin
              ran_something = true
              require prog
            rescue LoadError => e
              puts "Unable to find program '#{arg}' ('#{prog}'): #{e.message} (#{e.class})"
              exit! 1
            end
          end
        end
        exit 0
      end
    end
  end
  
  unless ran_something
    if Rubinius::Terminal
      repr = ENV['RBX_REPR'] || "bin/sirb"
      require repr
    else
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
    puts "An exception has occurred:"
    puts "    #{msg} (#{e.class})"
    puts "\nBacktrace:"
    puts e.backtrace.show
    code = 1
  rescue Object => e2
    puts "Unable to build backtrace due to errors"
    puts "Original Exception: #{e.inspect} (#{e.class})"
    puts "New Exception: #{e2.inspect} (#{e.class})"
    code = 128
  end
end

begin
  Rubinius::AtExit.each {|handler| handler.call}
rescue Object => e
  puts "An exception occurred inside an at_exit handler:"
  puts "    #{e.message} (#{e.class})"
  puts "\nBacktrace:"
  puts e.backtrace.show
  code = 1
end

Process.exit(code || 0)

