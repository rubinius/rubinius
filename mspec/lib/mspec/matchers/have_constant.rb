require 'mspec/matchers/stringsymboladapter'

class HaveConstantMatcher
  include StringSymbolAdapter

  def initialize(name)
    @name = convert_name name
  end

  def matches?(mod)
    @mod = mod
    @mod.constants.include? @name
  end

  def failure_message
    ["Expected #{@mod} to have constant '#{@name.to_s}'",
     "but it does not"]
  end

  def negative_failure_message
    ["Expected #{@mod} NOT to have constant '#{@name.to_s}'",
     "but it does"]
  end
end

class Object
  def have_constant(name)
    HaveConstantMatcher.new name
  end
end
