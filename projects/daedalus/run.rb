require 'daedalus'

file = ARGV.shift
command = (ARGV.shift || "build").to_s

bp = Daedalus.load(file)

begin
  bp.__send__ command, ARGV
rescue RuntimeError
  # ignore
end
