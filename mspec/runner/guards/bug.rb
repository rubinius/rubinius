require 'mspec/runner/guards/guard'

class BugGuard < SpecGuard
end

class Object
  def ruby_bug
    g = BugGuard.new
    yield if g.yield?
    g.unregister
  end
end
