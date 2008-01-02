module UnboundMethodSpecs
  class Methods
    def foo
      true
    end
    alias bar foo

    def one; end
    def two(a); end
    def three(a, b); end
    def four(a, b, &c); end
  
    def neg_one(*a); end
    def neg_two(a, *b); end
    def neg_three(a, b, *c); end
    def neg_four(a, b, *c, &d); end
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
