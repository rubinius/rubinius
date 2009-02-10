require 'mspec/guards/guard'

class CompliantOnGuard < SpecGuard
  def match?
    standard? or implementation?(*@args)
  end
end

class NotCompliantOnGuard < SpecGuard
  def match?
    standard? or !implementation?(*@args)
  end
end

class Object
  def compliant_on(*args)
    g = CompliantOnGuard.new(*args)
    yield if g.yield?
    g.unregister
  end

  def not_compliant_on(*args)
    g = NotCompliantOnGuard.new(*args)
    yield if g.yield?
    g.unregister
  end
end
