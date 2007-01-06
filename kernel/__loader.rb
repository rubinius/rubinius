# This is the end of the kernel and the beginning of specified
# code. We read out of ARGV to figure out what the user is
# trying to do.


# Parse options here!

# script = ARGV.shift

code = 0

begin
  until ARGV.empty?
    arg = ARGV.shift
    case arg
    when '-c'
      compile(ARGV.shift)
    when '-e'
      run_code(ARGV.shift)
    else
      load(arg)
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

