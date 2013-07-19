# -*- encoding: us-ascii -*-

class Dir

  # This seems silly, I know. But we do this to make Dir more resistent to people
  # screwing with ::File later (ie, fakefs)
  PrivateFile = ::File

  def self.open(path, options=undefined)
    dir = new path
    if block_given?
      begin
        value = yield dir
      ensure
        dir.close
      end

      return value
    else
      return dir
    end
  end

  def self.entries(path, options=undefined)
    ret = []

    open(path) do |dir|
      while s = dir.read
        ret << s
      end
    end

    ret
  end

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
