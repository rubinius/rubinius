require 'mspec/utils/version'

class HavePrivateInstanceMethodMatcher
  def initialize(method, include_super=true)
    @include_super = include_super
    version = SpecVersion.new(RUBY_VERSION) <=> "1.9"
    @method = version < 0 ? method.to_s : method
  end

  def matches?(mod)
    @mod = mod
    mod.private_instance_methods(@include_super).include? @method
  end

  def failure_message
    ["Expected #{@mod} to have private method '#{@method.to_s}'",
     "but it does not"]
  end

  def negative_failure_message
    ["Expected #{@mod} NOT to have private method '#{@method.to_s}'",
     "but it does"]
  end
end

class Object
  def have_private_instance_method(method, include_super=true)
    HavePrivateInstanceMethodMatcher.new method, include_super
  end
end
