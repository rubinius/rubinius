require 'mspec/guards/guard'

class ExtensionsGuard < SpecGuard
end

class Object
  def extended_on(*args)
    g = ExtensionsGuard.new(*args)
    yield if g.yield?
    g.unregister
  end
end
