# This is originally from the pty extension that comes with MRI
# by default.

$expect_verbose = false

class IO
  # Reads ios until pattern matches or the timeout is over. It returns
  # an array with the read buffer, followed by the matches. If a block is given,
  # the result is yielded to the block and returns nil. The optional timeout parameter defines,
  # in seconds, the total time to wait for pattern. If it is over of eof is found, it 
  # returns/yields nil. However, the buffer in a timeout session is kept for the next expect call.
  # The default timeout is 9999999 seconds.
  def expect(pat,timeout=9999999)
    case pat
    when String
      pat = Regexp.new Regexp.quote(pat)
    when Regexp
      # Nothing, we're good.
    else
      raise ArgumentError, "accepts a String or Regexp only"
    end

    buf = ''

    verbose = $expect_verbose

    while true
      if !IO.select([self], nil, nil, timeout) or eof?
        result = nil
        break
      end

      c = getc.chr
      buf << c

      if verbose
        STDOUT.print c
        STDOUT.flush
      end

      if match = pat.match(buf)
        result = [buf, *match.to_a[1..-1]]
        break
      end
    end

    return result unless block_given?

    yield result
    nil
  end
end

