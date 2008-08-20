require 'mspec/guards/guard'

class PlatformGuard < SpecGuard
  def initialize(*args)
    if args.last.is_a?(Hash)
      @options, @platforms = args.last, args[0..-2]
    else
      @options, @platforms = {}, args
    end
  end

  def match?
    match = @platforms.empty? ? true : platform?(*@platforms)
    @options.each do |key, value|
      case key
      when :os
        match &&= os?(*value)
      when :wordsize
        match &&= wordsize? value
      end
    end
    match
  end
end

class Object
  def platform_is(*args)
    g = PlatformGuard.new(*args)
    yield if g.yield?
    g.unregister
  end

  def platform_is_not(*args)
    g = PlatformGuard.new(*args)
    yield if g.yield? true
    g.unregister
  end
end
