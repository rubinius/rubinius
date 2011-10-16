class File
  def self.path(obj)
    return obj.to_path if obj.respond_to? :to_path

    StringValue(obj)
  end

  def size
    raise IOError, "closed stream" if closed?
    stat.size
  end

  def self.absolute_path(obj, dir = nil)
    if dir.nil?
      self.path(obj)
    else
      self.expand_path(obj, dir)
    end
  end

  alias_method :to_path, :path
end
