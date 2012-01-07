# -*- encoding: us-ascii -*-

class Dir
  def self.open(path)
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

  def self.entries(path)
    ret = []

    open(path) do |dir|
      while s = dir.read
        ret << s
      end
    end

    ret
  end
end
