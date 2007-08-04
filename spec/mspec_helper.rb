require 'mini_rspec'

def try(a, b=true)
  yield
  return nil
rescue a
  return b
end

def should_raise(klass = nil, msg = nil)
  raise ArgumentError, "No exception class given" unless klass

  begin
    yield
  rescue Exception => e
    unless klass === e
      raise ArgumentError, "Expected #{klass}, got #{e.class}"
    end
    
    if msg and msg != e.message then
      raise ArgumentError, "Expected message #{msg.inspect}, got #{e.message.inspect}"
    end
    
    return true
  end

  raise ArgumentError.new("Expected #{klass}, nothing raised")
end


# Adapted from rs
#
# Captures output from the IO given as the second argument (STDIN by default)
# and matches it against a String or Regexp given as the first argument.
def should_output(expected, to = STDOUT)
  require 'fileutils'
  require 'tmpdir'

  # Store the old stream
  old_to = to.dup

  # Obtain a filehandle to replace (needed for Readline support)
  to.reopen File.open(File.join(Dir.tmpdir, "should_output_#{$$}"), "w+")
  
  # Execute
  yield

  # Restore
  out = to.dup
  to.reopen old_to

  # Grab the data
  out.rewind
  output = out.read

  # Match up
  case expected
    when Regexp
      output.should.match expected
    else
      output.should.equal expected
  end                               # case expected

# Clean up
ensure
  out.close unless out.closed?

  # STDIO redirection will break without this
  begin
    to.seek 0, IO::SEEK_END
  rescue Errno::ESPIPE
  rescue Errno::EPIPE
  end

  FileUtils.rm out.path
end                                 # should_output
