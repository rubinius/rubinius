module MethodSpecs
  class Methods
    def foo
      true
    end
    alias bar foo

    attr_accessor :attr

    def zero; end
    def one_req(a); end
    def two_req(a, b); end

    def zero_with_block(&block); end
    def one_req_with_block(a, &block); end
    def two_req_with_block(a, b, &block); end

    def one_opt(a=nil); end
    def one_req_one_opt(a, b=nil); end
    def one_req_two_opt(a, b=nil, c=nil); end
    def two_req_one_opt(a, b, c=nil); end

    def one_opt_with_block(a=nil, &block); end
    def one_req_one_opt_with_block(a, b=nil, &block); end
    def one_req_two_opt_with_block(a, b=nil, c=nil, &block); end
    def two_req_one_opt_with_block(a, b, c=nil, &block); end

    def zero_with_splat(*a); end
    def one_req_with_splat(a, *b); end
    def two_req_with_splat(a, b, *c); end
    def one_req_one_opt_with_splat(a, b=nil, *c); end
    def two_req_one_opt_with_splat(a, b, c=nil, *d); end
    def one_req_two_opt_with_splat(a, b=nil, c=nil, *d); end

    def zero_with_splat_and_block(*a, &block); end
    def one_req_with_splat_and_block(a, *b, &block); end
    def two_req_with_splat_and_block(a, b, *c, &block); end
    def one_req_one_opt_with_splat_and_block(a, b=nil, *c, &block); end
    def two_req_one_opt_with_splat_and_block(a, b, c=nil, *d, &block); end
    def one_req_two_opt_with_splat_and_block(a, b=nil, c=nil, *d, &block); end
  end

  module MyMod
    def bar; :bar; end
  end

  class MySuper
    include MyMod
  end

  class MySub < MySuper; end

  class A
    def baz(a, b)
      self.class
    end
  end

  class B < A
  end

  class C < B
  end

  class D
    def bar() 'done' end
  end
end
