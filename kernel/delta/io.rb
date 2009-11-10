class IO::BidirectionalPipe
  (READ_METHODS + WRITE_METHODS).each do |method|
    undef_method method
  end
end
