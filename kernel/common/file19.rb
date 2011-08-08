class File
  def self.path(obj)
    return obj.to_path if obj.respond_to? :to_path

    StringValue(obj)
  end

  def size
    stat.size
  end

  alias_method :to_path, :path
end
