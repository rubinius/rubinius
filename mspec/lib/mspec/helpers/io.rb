class IOStub < String
  def write(*str)
    self << str.to_s
  end

  def print(*str)
    write(str.to_s + $\.to_s)
  end

  def puts(*str)
    write(str.collect { |s| s.to_s.chomp }.concat([nil]).join("\n"))
  end

  def flush
    self
  end
end
