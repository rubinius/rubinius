require 'mspec/guards/guard'

class SupportedGuard < SpecGuard
end

class Object
  def not_supported_on(*args)
    g = SupportedGuard.new(*args)
    yield if g.yield? true
    g.unregister
  end
end
