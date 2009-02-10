require 'mspec/guards/guard'

class NonComplianceGuard < SpecGuard
  def match?
    match = implementation?(*@args)
    raise Exception, "improper use of deviates_on guard" if match and standard?
    match
  end
end

class Object
  def deviates_on(*args)
    g = NonComplianceGuard.new(*args)
    yield if g.yield?
    g.unregister
  end
end
