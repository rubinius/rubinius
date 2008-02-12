module QuickGem
  # there can be more than one path
  PATHS = `ruby -rubygems -e "puts Gem.path"`.split

  @gems = nil
  def self.find_gems
    @gems = []
    PATHS.each do |path|
      path = path.strip + "/gems"
      Dir.foreach(path) do |name|
        @gems << "#{path}/#{name}"
      end
    end
    return @gems
  end

  def self.gems
    @gems ? @gems : find_gems
  end

  def self.load_gem(name)
    possible = gems.find_all { |g| /#{name}-.*/.match(g) }.sort
    if possible.empty?
      raise LoadError, "unknown gem '#{name}'"
    end

    $:.unshift "#{possible}/lib"
    if $DEBUG
      STDERR.puts "[Loaded gem #{name} with quickgem]"
    end
  end
end

module Kernel
  def gem(name, *rest)
    QuickGem.load_gem(name)
  end
end
