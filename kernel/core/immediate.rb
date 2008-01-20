# depends on: kernel.rb module.rb

module ImmediateValue
  def singleton_methods(all=true)
    []
  end
  
  def taint
    false
  end
  
  def tainted?
    false
  end
  
  def freeze
    false
  end
  
  def frozen?
    false
  end
  
end

class NilClass
  include ImmediateValue
end

class FalseClass
  include ImmediateValue
end

class TrueClass
  include ImmediateValue
end

class UndefClass
  include ImmediateValue
end

class Symbol
  include ImmediateValue
end

class Fixnum
  include ImmediateValue
end

