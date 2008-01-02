# depends on: class.rb

class ByteArray
  alias_method :[], :get_byte
  alias_method :[]=, :set_byte
  
  def each
    0.upto(self.size - 1) do |i|
      yield get_byte(i)
    end
  end
  
  def inspect
    "#<#{self.class}:0x#{object_id.to_s(16)} #{size} bytes>"
  end
  
end