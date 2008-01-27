module MSpec
  def self.protect(msg, &block)
    begin
      block.call
    rescue Exception => e
    end
  end
end
