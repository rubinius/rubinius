module MethodSpecs

  class Methods
    def one_splat_one_req(*a,b); end
    def one_splat_two_req(*a,b,c); end
    def one_splat_one_req_with_block(*a,b,&block); end

    def one_opt_with_stabby(a=->(b){true}); end
  end
end
