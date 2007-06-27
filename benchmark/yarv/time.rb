#!/usr/bin/env ruby
start_time = Time.now
child = fork {
	exec(*ARGV)
}
Process.waitpid(child)
if (!$?.success?)
	exit(1)
end	
end_time = Time.now
puts(end_time - start_time)
