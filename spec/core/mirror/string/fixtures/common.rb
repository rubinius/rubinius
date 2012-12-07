class Object
  def string_mirror(str)
    Rubinius::Mirror.reflect str
  end
end
