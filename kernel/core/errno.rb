module Errno
  def self.handle(additional = nil)
    err = Platform::POSIX.errno
    return if err == 0

    exc = Errno::Mapping[err]
    if exc
      msg = Platform::POSIX.strerror(err)

      unless additional.nil?
        msg << " - " << additional
      end

      raise exc.new(msg, err)
    else
      raise "Unknown error: #{Platform::POSIX.strerror(err)} (#{err})"
    end
  end
end
