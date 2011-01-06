class IO
  def self.binread(file, *arg)
    unless arg.size < 3
      raise ArgumentError, "wrong number of arguments (#{1+arg.size} for 1..3)"
    end

    File.open(file,"rb") do |f|
      f.read(*arg)
    end
  end
end
