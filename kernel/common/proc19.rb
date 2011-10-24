class Proc

  alias_method :===, :call

  def source_location
    [@block.file.to_s, @block.line]
  end
end
