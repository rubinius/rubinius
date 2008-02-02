require 'mspec/runner/guards/guard'

class NonComplianceGuard < SpecGuard
end

class Object
  def deviates_on(*args)
    g = NonCommplianceGuard.new(*args)
    yield if g.yield?
    g.unregister
  end
end
