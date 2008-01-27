module MSpec
  def self.protect(msg, &block)
    begin
      block.call
    rescue Exception => e
    end
  end
  
  def self.stack
    @stack ||= []
  end
  
  def self.current
    stack.last
  end
end
