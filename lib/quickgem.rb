module QuickGem
  PATH = `ruby -rubygems -e "puts Gem.path"`.strip + "/gems"

  @gems = nil
  def self.find_gems
    @gems = []
    Dir.foreach(PATH) do |name|
      @gems << name
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

    $:.unshift "#{PATH}/#{possible}/lib"
    if $DEBUG
      STDERR.puts "[Loaded gem #{name} with quickgem]"
    end

    return name
  end
end

module Kernel
  def gem(name, *rest)
    QuickGem.load_gem(name)
  end
end
