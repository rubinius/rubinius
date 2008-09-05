# depends on: io.rb

module Rubinius
  Terminal = STDIN.tty?
end

