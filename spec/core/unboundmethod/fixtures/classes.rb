module MethodSpecs


  class SourceLocation
    def location # This needs to be on this line
      :location  # for the spec to pass
    end

    def redefined
      :first
    end

    def redefined
      :last
    end

    def original
    end

    alias :aka :original
  end

  class Methods

    def zero; end

    def zero_with_locals
      number = 10
    end

    def one_req(a); end
    def two_req(a, b); end

    def zero_with_block(&block); end
    def one_req_with_block(a, &block); end
    def two_req_with_block(a, b, &block); end

    def zero_with_block_and_locals(&block)
      number = 10
    end

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

end
