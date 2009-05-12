class Dir
  def self.foreach(path)
    return to_enum :foreach, path unless block_given?
    self.open(path) do |dir|
      while s = dir.read
        yield s
      end
    end

    nil
  end
  
  def each
    return to_enum unless block_given?
    while s = read
      yield s
    end

    self
  end
end