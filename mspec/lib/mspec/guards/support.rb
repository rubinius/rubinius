require 'mspec/guards/guard'

class SupportedGuard < SpecGuard
  def match?
    if @args.include? :ruby
      raise Exception, "improper use of not_supported_on guard"
    end
    standard? or !implementation?(*@args)
  end
end

class Object
  def not_supported_on(*args)
    g = SupportedGuard.new(*args)
    yield if g.yield?
    g.unregister
  end
end
