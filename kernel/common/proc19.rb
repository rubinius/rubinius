class Proc

  alias_method :===, :call

  def source_location
    [@block.file.to_s, @block.line]
  end

  def to_s
    l = " (lambda)" if lambda?
    "#<#{self.class}:0x#{self.object_id.to_s(16)}@#{@block.file}:#{@block.line}#{l}>"
  end
end
