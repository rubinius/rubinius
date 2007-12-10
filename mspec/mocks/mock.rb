require 'mspec/expectations'

module Mock
  def self.reset
    @expects = {}
  end
  
  def self.expects
    @expects ||= {}
  end
  
  def self.set_expect(obj, sym)
    expects[[obj, sym]] = MockProxy.new
  end

  def self.replaced_name(sym)
    :"__ms_#{sym}__"
  end
  
  def self.install_method(obj, sym)
    meta = class << obj; self; end
    
    if obj.respond_to? sym
      meta.instance_eval { alias_method Mock.replaced_name(sym), sym.to_sym }
    end

    meta.class_eval <<-END
      def #{sym}(*args, &block)
        Mock.verify_call self, :#{sym}, *args, &block
      end
    END
    
    Mock.set_expect obj, sym
  end

  def self.verify_count
    expects.each do |key, proxy|
      obj, sym = key.first, key.last
      
      qualifier, count = proxy.count
      pass = case qualifier
      when :at_least
        proxy.calls >= count
      when :at_most
        proxy.calls <= count
      when :exactly
        proxy.calls == count
      else
        false
      end
      unless pass
        Expectation.fail_with("Mock #{obj.inspect}\nexpected to receive #{sym} #{qualifier.to_s.sub('_', ' ')} #{count} times",
                              "but received it #{proxy.calls} times")
      end
    end
  end

  def self.cleanup
    expects.keys.each do |obj, sym|
      meta = class << obj; self; end
      replaced = Mock.replaced_name(sym)
      if obj.respond_to?(replaced)
        meta.instance_eval { alias_method sym.to_sym, replaced }
        meta.send :remove_method, replaced
      else
        meta.send :remove_method, sym.to_sym
      end
    end
    reset
  end  

  def self.verify_call(obj, sym, *args, &block)
    proxy = expects[[obj, sym]]
    
    compare = *args
    unless proxy.arguments == compare
      Expectation.fail_with("Mock #{obj.inspect}\nexpected #{sym} with: (#{proxy.arguments.inspect})",
                            "             but received: (#{compare.inspect})")
    end
    
    proxy.called
    return proxy.returning
  end
end
