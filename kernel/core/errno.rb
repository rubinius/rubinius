module Errno
  def self.handle(msg=nil)
    err = Platform::POSIX.errno
    return if err == 0
    
    exc = Errno::Mapping[err]
    if exc
      if msg
        msg = "#{msg} (#{Platform::POSIX.strerror})"
      else
        msg = Platform::POSIX.strerror
      end
      raise exc.new(msg, err)
    else
      raise "Unknown error: #{Platform::POSIX.strerror} (#{err})"
    end
  end
end
