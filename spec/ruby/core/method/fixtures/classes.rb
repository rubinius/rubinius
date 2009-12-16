module MethodSpecs
  
  
  class SourceLocation
    def self.location # This needs to be on this line 
      :location       # for the spec to pass
    end
    
    def self.redefined
      :first
    end

    def self.redefined
      :last
    end

    def original
    end

    alias :aka :original 
  end

  class Methods
    def foo
      true
    end

    alias bar foo

    def same_as_foo
      true
    end

    def respond_to_missing? method, bool
      [:handled_via_method_missing, :also_handled].include? method
    end

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

  class Eql

    def same_body
      1 + 1
    end

    alias :same_body_alias :same_body

    def same_body_with_args(arg)
      1 + 1
    end

    def different_body
      1 + 2
    end

    def same_body_two 
      1 + 1
    end

    private
    def same_body_private
      1 + 1
    end
  end

  class Eql2

    def same_body
      1 + 1
    end

  end
end
