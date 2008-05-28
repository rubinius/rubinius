require 'mspec/expectations/expectations'

module Mock
  def self.reset
    @expects = nil
  end

  def self.expects
    @expects ||= Hash.new { |h,k| h[k] = [] }
  end

  def self.replaced_name(sym)
    :"__ms_#{sym}__"
  end

  def self.install_method(obj, sym, type = :mock)
    meta = class << obj; self; end

    if (sym.to_sym == :respond_to? or obj.respond_to?(sym)) and !meta.instance_methods.include?(replaced_name(sym).to_s)
      meta.__send__ :alias_method, replaced_name(sym), sym.to_sym
    end

    meta.class_eval <<-END
      def #{sym}(*args, &block)
        Mock.verify_call self, :#{sym}, *args, &block
      end
    END

    MSpec.actions :expectation, MSpec.current.state

    proxy = MockProxy.new

    if type == :stub
      expects[[obj, sym]] << proxy
      proxy.at_least(0)
    else
      expects[[obj, sym]].unshift proxy
      proxy.exactly(1)
    end
  end

  def self.verify_count
    expects.each do |key, proxies|
      obj, sym = key.first, key.last

      proxies.each do |proxy|
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
  end

  def self.verify_call(obj, sym, *args, &block)
    compare = *args

    expects[[obj, sym]].each do |proxy|
      pass = case proxy.arguments
      when :any_args
        true
      when :no_args
        compare.nil?
      else
        proxy.arguments == compare
      end
      
      if proxy.yielding?
        if block
          proxy.yielding.each do |args_to_yield|
            if block.arity == -1 || block.arity == args_to_yield.size
              block.call(*args_to_yield)
            else
              Expectation.fail_with("Mock #{obj.inspect} asked to yield |#{proxy.yielding.join(', ')}| on #{sym}\n",
                                    "but a block with arity #{block.arity} was passed")
            end
          end
        else
          Expectation.fail_with("Mock #{obj.inspect} asked to yield |[#{proxy.yielding.join('], [')}]| on #{sym}\n",
                                "but no block was passed")
        end
      end

      if pass
        proxy.called
        return proxy.returning
      end
    end

    if sym.to_sym == :respond_to?
      return obj.__send__(replaced_name(sym), compare)
    else
      Expectation.fail_with("Mock #{obj.inspect}: method #{sym}\n",
                            "called with unexpected arguments (#{Array(compare).join(' ')})")
    end
  end

  def self.cleanup
    expects.keys.each do |obj, sym|
      meta = class << obj; self; end

      replaced = replaced_name(sym)
      if meta.instance_methods.include?(replaced.to_s)
        meta.__send__ :alias_method, sym.to_sym, replaced
        meta.__send__ :remove_method, replaced
      else
        meta.__send__ :remove_method, sym.to_sym
      end
    end
    reset
  end
end
