require 'mspec/runner/guards/guard'

class BugGuard < SpecGuard
  def match?
    not implementation?(:ruby, :ruby18, :ruby19)
  end
end

class Object
  def ruby_bug
    g = BugGuard.new
    yield if g.yield?
    g.unregister
  end
end
