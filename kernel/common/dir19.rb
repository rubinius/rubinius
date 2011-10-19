class Dir
  def self.exists?(path)
    File.directory?(path)
  end

  class << self
    alias_method :exist?, :exists?
  end

  def self.home(user=nil)
    File.expand_path("~#{user}")
  end

  alias_method :to_path, :path
end
