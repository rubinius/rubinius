require 'mspec/guards/guard'

class ConflictsGuard < SpecGuard
  def match?
    constants = Object.constants
    @args.any? { |mod| constants.include? mod.to_s }
  end
end

class Object
  def conflicts_with(*modules)
    g = ConflictsGuard.new(*modules)
    yield if g.yield? true
    g.unregister
  end
end
