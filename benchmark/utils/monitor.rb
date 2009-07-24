# This script provides a place to insert a potentially platform-specific
# method of monitoring a benchmark and possibly aborting the run if it
# exceeds a specified time limit. See README for more details.

def write_status(name, report, status)
  File.open report, "a" do |f|
    f.puts "---"
    f.puts "name: #{name}"
    f.puts "status: #{status}"
  end
end

Process.setpgrp

Signal.trap :TERM do
  # fuck off
end

limit, vm, runner, name, iterations, report = ARGV
finish = Time.now.to_i + limit.to_i

if pid = Kernel.fork
  loop do
    Process.waitpid pid, Process::WNOHANG

    if status = $?
      if status.success?
        write_status name, report, "success"
        exit 0
      end

      write_status name, report, "Exited with status #{status.exitstatus}"
      exit 1
    end

    if Time.now.to_i > finish
      write_status name, report, "Timeout"

      Process.kill :TERM, 0
      Process.waitpid pid, Process::WNOHANG

      sleep 2
      Process.kill :KILL, 0
      Process.waitpid pid
      exit 1
    end

    sleep 1
  end
else
  args = vm.split
  exe = args.shift
  args.concat [runner, name, iterations, report]
  exec exe, *args
end
