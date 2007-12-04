require 'mspec/mocks/proxy'

class Object
  # Provide the method name and a hash with any of the following
  #   :with       => Array of arguments or :any (default)
  #   :block      => Whether block is present or :any (default)
  #   :count      => Number of times invoked, default once (special: :any and :never)
  #   :returning  => Object to return
  def should_receive(sym)
    __mock_install_method sym
    
    # info[:with]   = info[:with] || :any
    # info[:block]  = info[:block] || :any
    # info[:count]  = info[:count] || 1
    # 
    Mock.set_expect self, sym
  end

  # Same as should_receive except that :count is 0
  def should_not_receive(sym)
    __mock_install_method(sym)
    proxy = Mock.set_expect self, sym
    proxy.exactly(0).times
    # return nil so that further chained calls will raise
    nil
  end
  
  private
  def __mock_install_method(sym)
    meta = class << self; self; end
    
    if self.respond_to? sym
      meta.instance_eval { alias_method(:"__ms_#{sym}", sym.to_sym) }
      Mock.set_objects self, sym, :single_overridden 
    else 
      Mock.set_objects self, sym, :single_new 
    end

    meta.class_eval <<-END
      def #{sym}(*args, &block)
        Mock.report self, :#{sym}, *args, &block
      end
    END
  end
end
