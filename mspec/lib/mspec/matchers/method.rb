require 'mspec/utils/version'

class MethodMatcher
  def initialize(method, include_super=true)
    @include_super = include_super
    version = SpecVersion.new(RUBY_VERSION) <=> "1.9"
    @method = version < 0 ? method.to_s : method
  end

  def matches?(mod)
    raise Exception, "define #matches? in the subclass"
  end
end
