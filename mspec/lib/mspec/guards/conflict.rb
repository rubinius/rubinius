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
    g.name = :conflicts_with
    yield if g.yield? true
  ensure
    g.unregister
  end
end
