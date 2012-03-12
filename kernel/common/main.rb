# -*- encoding: us-ascii -*-

class << MAIN
  def include(*mods)
    Rubinius.privately do
      Object.include(*mods)
    end
  end

  def public(*methods)
    Rubinius.privately do
      Object.public(*methods)
    end
  end

  def private(*methods)
    Rubinius.privately do
      Object.private(*methods)
    end
  end

  def to_s
    "main"
  end

  alias_method :inspect, :to_s
end
