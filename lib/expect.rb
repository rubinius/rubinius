# This is originally from the pty extension that comes with MRI
# by default.
#
# Documentation by Mike Ballou and Brian Miller on 06/21/2011
$expect_verbose = false

class IO
  # == Overview
  # expect(match) listens to an IO input stream and returns the contents when
  # the match is found. If a block is provided, it returns the block applied
  # to the string
  #  
  # +pat+ is the pattern that will be matched against the input. +pat+ can be
  # a string or a Regexp
  # +timeout+ is the amount of time the method will try to read from the IO
  # object before returning. Leaving the +timeout+ to +nil+ causes the read to
  # block infinitely until an event transpires.
  #
  # expect can take a block that will be executed if the input read matches
  # the pattern provided.
  #
  # == Examples of use
  # 
  # === With a block
  #
  #   STDIN.expect(/stop/) do |m|
  #     puts "For the love of god " + m
  #   end
  #
  # === Without a block
  #
  #   STDIN.expect(/stop/)
  
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

