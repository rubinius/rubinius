class IO
  remove_method :orig_reopen
end

class IO::BidirectionalPipe
  (READ_METHODS + WRITE_METHODS).each do |method|
    undef_method method
  end
end

# Re-setup all the stdio channels, to pull in new ivars

STDOUT.setup
STDIN.setup
STDERR.setup

