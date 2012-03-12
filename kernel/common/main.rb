# -*- encoding: us-ascii -*-

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

  def to_s
    "main"
  end

  alias_method :inspect, :to_s
end
