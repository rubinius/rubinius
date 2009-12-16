module DelegateSpecs
  class Simple
    def pub
      :foo
    end
    
    def respond_to_missing?(method, priv=false)
      method == :pub_too ||
        (priv && method == :priv_too)
    end
    
    def method_missing(method, *args)
      super unless respond_to_missing?(method, true)
      method
    end

    def priv(arg=nil)
      yield arg if block_given?
      [:priv, arg]
    end
    private :priv
    
  end
end
