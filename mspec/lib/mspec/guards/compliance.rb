require 'mspec/guards/guard'

class ComplianceGuard < SpecGuard
end

class Object
  def compliant_on(*args)
    g = ComplianceGuard.new(*args)
    yield if g.yield?
    g.unregister
  end

  def not_compliant_on(*args)
    g = ComplianceGuard.new(*args)
    yield if g.yield? true
    g.unregister
  end
end
