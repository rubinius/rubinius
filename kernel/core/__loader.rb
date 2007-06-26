# This is the end of the kernel and the beginning of specified
# code. We read out of ARGV to figure out what the user is
# trying to do.

GC.start

# Now, setup a few changes to the include path.

# If there is a closer compiler, use it. Otherwise, use the system one.
unless File.exists? "runtime/compiler.rba"
  $:.unshift "#{Rubinius::RBA_PATH}/compiler.rba"
end

# The main stdlib location
$: << Rubinius::CODE_PATH

# Parse options here!
RBS_USAGE = <<END
Usage: rubinius [options] [file]
  File may be any valid Ruby source file (.rb) or a compiled Ruby file (.rbc).

  Options: 
          -c   Compile file only.
          -e   Directly compile and execute code that follows (no file provided). 

          -dc  Display debugging information for compiler.
          -dl  Display debugging information for loader.
END

# script = ARGV.shift

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
      puts "rubinius #{Rubinius::VERSION} (#{Rubinius::BUILDREV}) (#{RUBY_RELEASE_DATE}) [#{RUBY_PLATFORM}]"
      $VERBOSE = true
    when '-dc'
      puts "[Compiler debugging enabled]"
      $DEBUG_COMPILER = true
    when '-dl'
      $DEBUG_LOADING = true
      puts "[Code loading debugging enabled]"
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
    puts "An exception has occurred:"
    puts "    #{e.message} (#{e.class})"
    puts "\nBacktrace:"
    puts e.backtrace.show
    code = 1
  rescue Object => e
    puts "VERY BROKEN."
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

