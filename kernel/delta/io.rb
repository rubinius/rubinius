class IO
  remove_method :orig_reopen
end

class IO::BidirectionalPipe
  (READ_METHODS + WRITE_METHODS).each do |method|
    undef_method method
  end
end
