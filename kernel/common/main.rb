class << MAIN
  def include(*mods)
    Object.include(*mods)
  end

  def public(*methods)
    Object.public(*methods)
  end

  def private(*methods)
    Object.private(*methods)
  end

  def protected(*methods)
    Object.protected(*methods)
  end

  def add_method(name, obj)
    Object.add_method(name, obj)
  end

  def alias_method(new_name, current_name)
    Object.__send__ :alias_method, new_name, current_name
  end

  def const_set(name, value)
    Object.const_set name, value
  end

  def to_s
    "main"
  end

  alias_method :inspect, :to_s
end
