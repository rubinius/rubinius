require 'mspec/guards/guard'

class ExtensionsGuard < SpecGuard
  def match?
    match = implementation?(*@args)
    raise Exception, "improper use of extended_on guard" if match and standard?
    match
  end
end

class Object
  def extended_on(*args)
    g = ExtensionsGuard.new(*args)
    yield if g.yield?
    g.unregister
  end
end
