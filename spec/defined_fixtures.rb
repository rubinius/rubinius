# for super and zsuper test cases
class Foo
  def no_args
  end
  def args(x)
  end
end

class Bar < Foo
  def no_args
    defined?(super)
  end
  def args
    defined?( super() )
  end
end

