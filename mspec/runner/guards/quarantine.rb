require 'mspec/runner/guards/guard'

class QuarantineGuard < SpecGuard
  def self.after
  end
  
  def before
  end
  
  def after
  end
end

class Object
  def quarantine!
    g = QuarantineGuard.new
    yield if g.yield?
    g.unregister
  end
end
