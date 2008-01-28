module MSpec
  def self.protect(msg, &block)
    begin
      block.call
    rescue Exception => e
      if current and current.state
        current.state.exceptions << [msg, e]
      else
        STDERR.write "An exception occurred in #{msg}: #{e.class}: #{e.message}"
      end
    end
  end
  
  def self.stack
    @stack ||= []
  end
  
  def self.current
    stack.last
  end
end
