class Dir

  # This seems silly, I know. But we do this to make Dir more resistent to people
  # screwing with ::File later (ie, fakefs)
  PrivateFile = ::File

  def self.exists?(path)
    PrivateFile.directory?(path)
  end

  class << self
    alias_method :exist?, :exists?
  end

  def self.home(user=nil)
    PrivateFile.expand_path("~#{user}")
  end

  alias_method :to_path, :path
end
