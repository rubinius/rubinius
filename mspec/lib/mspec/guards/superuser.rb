require 'mspec/guards/guard'

class SuperUserGuard < SpecGuard
  def match?
    Process.euid == 0
  end
end

class Object
  def as_superuser
    g = SuperUserGuard.new
    yield if g.yield?
    g.unregister
  end
end
