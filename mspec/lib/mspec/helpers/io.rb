class IOStub < String
  def write(*str)
    self << str.join
  end

  def print(*str)
    write(str.join + $\.to_s)
  end

  def puts(*str)
    write(str.collect { |s| s.to_s.chomp }.concat([nil]).join("\n"))
  end

  def printf(format, *args)
    self << sprintf(format, *args)
  end

  def flush
    self
  end
end
