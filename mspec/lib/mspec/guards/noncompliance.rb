require 'mspec/guards/guard'

class NonComplianceGuard < SpecGuard
  def match?
    if @args.include? :ruby
      raise Exception, "improper use of deviates_on guard"
    end
    !standard? and implementation?(*@args)
  end
end

class Object
  def deviates_on(*args)
    g = NonComplianceGuard.new(*args)
    yield if g.yield?
    g.unregister
  end
end
