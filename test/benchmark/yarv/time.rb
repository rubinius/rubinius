#!/usr/bin/env ruby

max = 60

def fin(code=0)
  Process.wait(@child)
  if !$?.success?
    exit! 1
  end
  exit! code
end

@child = fork {
	exec(*ARGV)
}

Signal.trap('CHLD') do |o|
  fin
end

sleep max
Process.kill 'INT', @child
exit! 2
