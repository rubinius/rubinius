# This is the end of the kernel and the beginning of specified
# code. We read out of ARGV to figure out what the user is
# trying to do.


# Parse options here!

script = ARGV.shift

begin
  meth = CompiledMethod.load_from_file(script)
rescue Object => e
  puts "Unable to load '#{script}'."
  return
end

begin
  meth.activate_as_script
rescue Object => e
  puts "An exception has occured:"
  puts "    #{e.message} (#{e.class})"
  puts "\nBacktrace:"
  puts e.backtrace.show
  exit(1)
end