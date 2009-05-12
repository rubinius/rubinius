# These methods are overriden by lib/1.8.7/... or lib/1.9/...
class Dir
  def self.foreach(path)
    self.open(path) do |dir|
      while s = dir.read
        yield s
      end
    end

    nil
  end

  def each
    while s = read
      yield s
    end

    self
  end

end