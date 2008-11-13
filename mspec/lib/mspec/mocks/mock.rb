require 'mspec/expectations/expectations'

module Mock
  def self.reset
    @mocks = @stubs = nil
  end

  def self.mocks
    @mocks ||= Hash.new { |h,k| h[k] = [] }
  end

  def self.stubs
    @stubs ||= Hash.new { |h,k| h[k] = [] }
  end

  def self.replaced_name(obj, sym)
    :"__ms_#{obj.__id__}_#{sym}__"
  end

  def self.replaced_key(obj, sym)
    [replaced_name(obj, sym), obj, sym]
  end

  def self.replaced?(key)
    !!(mocks.keys + stubs.keys).find { |k| k.first == key.first }
  end

  def self.install_method(obj, sym, type=nil)
    meta = class << obj; self; end

    key = replaced_key obj, sym
    if (sym.to_sym == :respond_to? or obj.respond_to?(sym)) and !replaced?(key)
      meta.__send__ :alias_method, key.first, sym.to_sym
    end

    meta.class_eval <<-END
      def #{sym}(*args, &block)
        Mock.verify_call self, :#{sym}, *args, &block
      end
    END

    proxy = MockProxy.new type

    if proxy.mock?
      MSpec.expectation
      MSpec.actions :expectation, MSpec.current.state
    end

    if proxy.stub?
      stubs[key].unshift proxy
    else
      mocks[key] << proxy
    end

    proxy
  end

  def self.name_or_inspect(obj)
    obj.instance_variable_get(:@name) || obj.inspect
  end

  def self.verify_count
    mocks.each do |key, proxies|
      replaced, obj, sym = *key
      proxies.each do |proxy|
        qualifier, count = proxy.count
        pass = case qualifier
        when :at_least
          proxy.calls >= count
        when :at_most
          proxy.calls <= count
        when :exactly
          proxy.calls == count
        when :any_number_of_times
          true
        else
          false
        end
        unless pass
          Expectation.fail_with(
            "Mock '#{name_or_inspect obj}' expected to receive '#{sym}' " \
            "#{qualifier.to_s.sub('_', ' ')} #{count} times",
            "but received it #{proxy.calls} times")
        end
      end
    end
  end

  def self.verify_call(obj, sym, *args, &block)
    compare = *args
    if RUBY_VERSION >= '1.9'
      compare = compare.first if compare.length <= 1
    end

    key = replaced_key obj, sym
    proxies = mocks[key] + stubs[key]
    proxies.each do |proxy|
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
              Expectation.fail_with(
                "Mock '#{name_or_inspect obj}' asked to yield " \
                "|#{proxy.yielding.join(', ')}| on #{sym}\n",
                "but a block with arity #{block.arity} was passed")
            end
          end
        else
          Expectation.fail_with(
            "Mock '#{name_or_inspect obj}' asked to yield " \
            "|[#{proxy.yielding.join('], [')}]| on #{sym}\n",
            "but no block was passed")
        end
      end

      if pass
        proxy.called
        return proxy.returning
      end
    end

    if sym.to_sym == :respond_to?
      return obj.__send__(replaced_name(obj, sym), compare)
    else
      Expectation.fail_with("Mock '#{name_or_inspect obj}': method #{sym}\n",
                            "called with unexpected arguments (#{Array(compare).join(' ')})")
    end
  end

  def self.cleanup
    symbols = mocks.keys + stubs.keys
    symbols.uniq.each do |replaced, obj, sym|
      meta = class << obj; self; end

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
