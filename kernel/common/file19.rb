class File
  def size
    stat.size
  end

  alias_method :to_path, :path
end
