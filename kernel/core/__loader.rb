# This is the end of the kernel and the beginning of specified
# code. We read out of ARGV to figure out what the user is
# trying to do.

GC.start

# Now, setup a few changes to the include path.

$:.unshift "#{Rubinius::RBA_PATH}/compiler.rba"
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

code = 0

begin
  until ARGV.empty?
    arg = ARGV.shift
    case arg
    when '-h'
      puts RBS_USAGE
    when "-v"
      puts "rubinius #{Rubinius::VERSION} (#{Rubinius::BUILDREV}) (#{RUBY_RELEASE_DATE}) [#{RUBY_PLATFORM}]"
    when '-dc'
      puts "[Compiler debugging enabled]"
      $DEBUG_COMPILER = true
    when '-dl'
      $DEBUG_LOADING = true
      puts "[Code loading debugging enabled]"
    when '-c'
      file = ARGV.shift
      path = compile(file)
      puts "Compiled #{file} to #{path}"
    when '-e'
      Compile.execute ARGV.shift
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
      elsif arg.prefix? "-"
        puts "Invalid switch '#{arg}'"
        exit! 1
      else
        if File.exists?(arg)
          load(arg)
        else
          if arg.suffix?(".rb")
            puts "Unable to find '#{arg}'"
            exit! 1
          else
            prog = "bin/#{arg}"
            begin
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
rescue SystemExit => e
  code = e.code
rescue Object => e
  puts "An exception has occured:"
  puts "    #{e.message} (#{e.class})"
  puts "\nBacktrace:"
  puts e.backtrace.show
  code = 1
end

begin
  Ruby::AtExit.each {|handler| handler.call}
rescue Object => e
  puts "An exception occured inside an at_exit handler:"
  puts "    #{e.message} (#{e.class})"
  puts "\nBacktrace:"
  puts e.backtrace.show
  code = 1
end

Process.exit(code)

