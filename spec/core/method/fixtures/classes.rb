module MethodSpecs
  class Methods
    def foo
      true
    end
    alias bar foo
  
    def zero; end
    def one(a); end
    def two(a, b); end
  
    def neg_one(*a); end
    def neg_two(a, *b); end
    def neg_three(a, b, *c, &d); end
  end

  class A
    def baz(a, b)
      self.class
    end
  end

  class B < A
  end

  class C < B
  end
end
