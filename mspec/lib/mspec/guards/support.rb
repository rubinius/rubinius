require 'mspec/guards/guard'

class SupportedGuard < SpecGuard
  def match?
    match = implementation?(*@args)
    raise Exception, "improper use of not_supported_on guard" if match and standard?
    match
  end
end

class Object
  def not_supported_on(*args)
    g = SupportedGuard.new(*args)
    yield if g.yield? true
    g.unregister
  end
end
