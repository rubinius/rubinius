# This is the end of the kernel and the beginning of specified
# code. We read out of ARGV to figure out what the user is
# trying to do.

GC.start

# Parse options here!

# script = ARGV.shift

code = 0

begin
  until ARGV.empty?
    arg = ARGV.shift
    case arg
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
      out = Compile.execute ARGV.shift
      puts "\nOutput: #{out.inspect}"
    else
      if arg.prefix? "-I"
        arg[2..-1].split(":").each do |path|
          $:.unshift(path)
        end
      elsif arg.prefix? "-r"
        require(arg[2..-1])
      else
        load(arg)
      end
    end
  end
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

exit(code)

