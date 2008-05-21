require 'mspec/guards/guard'

class QuarantineGuard < SpecGuard
  def match?
    false
  end
end

class Object
  def quarantine!
    g = QuarantineGuard.new
    yield if g.yield?
    g.unregister
  end
end
