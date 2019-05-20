class Enumerator
  include Enumerable

  attr_writer :args
  private :args=

  attr_writer :size
  private :size=

  attr_reader :generator
  protected :generator

  attr_reader :lookahead
  protected :lookahead

  def initialize_enumerator(receiver, size, method_name, *method_args)
    @object = receiver
    @size = size
    @iter = method_name
    @args = method_args
    @generator = nil
    @lookahead = []
    @feedvalue = nil

    self
  end
  private :initialize_enumerator

  def initialize(receiver_or_size=undefined, method_name=:each, *method_args, &block)
    size = nil

    if block_given?
      unless undefined.equal? receiver_or_size
        size = if receiver_or_size.nil? || receiver_or_size == Float::INFINITY || receiver_or_size.respond_to?(:call)
                  receiver_or_size
                else
                  Rubinius::Type.coerce_to receiver_or_size, Integer, :to_int
                end
      end

      receiver = Generator.new(&block)
    else
      if undefined.equal? receiver_or_size
        raise ArgumentError, "Enumerator#initialize requires a block when called without arguments"
      end

      receiver = receiver_or_size
    end

    method_name = Rubinius::Type.coerce_to method_name, Symbol, :to_sym

    initialize_enumerator receiver, size, method_name, *method_args

    self
  end
  private :initialize

  def each(*args)
    enumerator = self
    new_args = @args

    unless args.empty?
      enumerator = dup
      new_args = @args.empty? ? args : (@args + args)
    end

    Rubinius.privately do
      enumerator.args = new_args
    end

    if block_given?
      Rubinius.privately do
        enumerator.each_with_block do |*yield_args|
          yield(*yield_args)
        end
      end
    else
      enumerator
    end
  end

  def each_with_block
    @object.__send__ @iter, *@args do |*args|
      ret = yield(*args)
      unless @feedvalue.nil?
        ret = @feedvalue
        @feedvalue = nil
      end
      ret
    end
  end
  private :each_with_block

  def each_with_index
    return to_enum(:each_with_index) { size } unless block_given?

    idx = 0

    each do
      o = Rubinius.single_block_arg
      val = yield(o, idx)
      idx += 1
      val
    end
  end

  def next
    return @lookahead.shift unless @lookahead.empty?

    @generator ||= Iterator.new self

    begin
      return @generator.next if @generator.next?
    rescue StopIteration
    end

    exception = StopIteration.new "iteration reached end"
    Rubinius.privately do
      exception.result = @generator.result
    end

    raise exception
  end

  def next_values
    Array(self.next)
  end

  def peek
    return @lookahead.first unless @lookahead.empty?
    item = self.next
    @lookahead << item
    item
  end

  def peek_values
    Array(self.peek).dup
  end

  def rewind
    @object.rewind if @object.respond_to? :rewind
    @generator.rewind if @generator
    @lookahead = []
    @feedvalue = nil
    self
  end

  def size
    @size.respond_to?(:call) ? @size.call : @size
  end

  def with_index(offset=0)
    if offset
      offset = Rubinius::Type.coerce_to offset, Integer, :to_int
    else
      offset = 0
    end

    return to_enum(:with_index, offset) { size } unless block_given?

    each do
      o = Rubinius.single_block_arg
      val = yield(o, offset)
      offset += 1
      val
    end
  end

  def feed(val)
    raise TypeError, "Feed value already set" unless @feedvalue.nil?
    @feedvalue = val
    nil
  end

  def dup
    raise TypeError, "Can't copy iteration state" if @generator&.instance_variable_get(:@fiber)
    super
  end

  def initialize_copy(other)
    @generator = other.generator&.dup
    @lookahead = other.lookahead.dup
    super
  end
  private :initialize_copy

  class Yielder
    def initialize(&block)
      raise LocalJumpError, "Expected a block to be given" unless block_given?

      @proc = block

      self
    end
    private :initialize

    def yield(*args)
      @proc.call *args
    end

    def <<(*args)
      self.yield(*args)

      self
    end
  end

  class Generator
    include Enumerable

    def initialize(&block)
      raise LocalJumpError, "Expected a block to be given" unless block_given?

      @proc = block

      self
    end
    private :initialize

    def each(*args)
      enclosed_yield = Proc.new { |*enclosed_args| yield *enclosed_args }

      @proc.call Yielder.new(&enclosed_yield), *args
    end
  end

  class Lazy < self
    class StopLazyError < Exception; end

    def initialize(receiver, size=nil)
      raise ArgumentError, "Lazy#initialize requires a block" unless block_given?
      Rubinius.check_frozen

      super(size) do |yielder, *each_args|
        begin
          receiver.each(*each_args) do |*args|
            yield yielder, *args
          end
        rescue Exception
          nil
        end
      end

      self
    end
    private :initialize

    def to_enum(method_name=:each, *method_args, &block)
      size = block_given? ? block : nil
      ret = Lazy.allocate

      Rubinius.privately do
        ret.initialize_enumerator self, size, method_name, *method_args
      end

      ret
    end
    alias_method :enum_for, :to_enum

    def lazy
      self
    end

    alias_method :force, :to_a

    def take(n)
      n = Rubinius::Type.coerce_to n, Integer, :to_int
      raise ArgumentError, "attempt to take negative size" if n < 0

      current_size = enumerator_size
      set_size = if current_size.kind_of?(Numeric)
        n < current_size ? n : current_size
      else
        current_size
      end

      return to_enum(:cycle, 0).lazy if n.zero?

      taken = 0
      Lazy.new(self, set_size) do |yielder, *args|
        if taken < n
          yielder.yield(*args)
          taken += 1
          raise StopLazyError unless taken < n
        else
          raise StopLazyError
        end
      end
    end

    def drop(n)
      n = Rubinius::Type.coerce_to n, Integer, :to_int
      raise ArgumentError, "attempt to drop negative size" if n < 0

      current_size = enumerator_size
      set_size = if current_size.kind_of?(Integer)
        n < current_size ? current_size - n : 0
      else
        current_size
      end

      dropped = 0
      Lazy.new(self, set_size) do |yielder, *args|
        if dropped < n
          dropped += 1
        else
          yielder.yield(*args)
        end
      end
    end

    def take_while
      raise ArgumentError, "Lazy#take_while requires a block" unless block_given?

      Lazy.new(self, nil) do |yielder, *args|
        if yield(*args)
          yielder.yield(*args)
        else
          raise StopLazyError
        end
      end
    end

    def drop_while
      raise ArgumentError, "Lazy#drop_while requires a block" unless block_given?

      succeeding = true
      Lazy.new(self, nil) do |yielder, *args|
        if succeeding
          unless yield(*args)
            succeeding = false
            yielder.yield(*args)
          end
        else
          yielder.yield(*args)
        end
      end
    end

    def select
      raise ArgumentError, 'Lazy#{select,find_all} requires a block' unless block_given?

      Lazy.new(self, nil) do |yielder, *args|
        val = args.length >= 2 ? args : args.first
        yielder.yield(*args) if yield(val)
      end
    end
    alias_method :find_all, :select

    def reject
      raise ArgumentError, "Lazy#reject requires a block" unless block_given?

      Lazy.new(self, nil) do |yielder, *args|
        val = args.length >= 2 ? args : args.first
        yielder.yield(*args) unless yield(val)
      end
    end

    def grep(pattern)
      Lazy.new(self, nil) do |yielder, *args|
        val = args.length >= 2 ? args : args.first

        if pattern === val
          Regexp.set_block_last_match

          if block_given?
            yielder.yield yield(val)
          else
            yielder.yield val
          end
        end
      end
    end

    def grep_v(pattern)
      Lazy.new(self, nil) do |yielder, *args|
        val = args.length >= 2 ? args : args.first

        unless pattern === val
          if block_given?
            yielder.yield yield(val)
          else
            yielder.yield val
          end
        end
      end
    end

    def map
      raise ArgumentError, 'Lazy#{map,collect} requires a block' unless block_given?

      Lazy.new(self, enumerator_size) do |yielder, *args|
        yielder.yield yield(*args)
      end
    end
    alias_method :collect, :map

    def collect_concat
      raise ArgumentError, 'Lazy#{collect_concat,flat_map} requires a block' unless block_given?

      Lazy.new(self, nil) do |yielder, *args|
        yield_ret = yield(*args)

        if Rubinius::Type.object_respond_to?(yield_ret, :force) &&
            Rubinius::Type.object_respond_to?(yield_ret, :each)
          yield_ret.each do |v|
            yielder.yield v
          end
        else
          array = Rubinius::Type.check_convert_type yield_ret, Array, :to_ary
          if array
            array.each do |v|
              yielder.yield v
            end
          else
            yielder.yield yield_ret
          end
        end
      end
    end
    alias_method :flat_map, :collect_concat

    def zip(*lists)
      return super(*lists) { |entry| yield entry } if block_given?

      lists.map! do |list|
        array = Rubinius::Type.check_convert_type list, Array, :to_ary

        case
        when array
          array
        when Rubinius::Type.object_respond_to?(list, :each)
          list.to_enum :each
        else
          raise TypeError, "wrong argument type #{list.class} (must respond to :each)"
        end
      end

      index = 0
      Lazy.new(self, enumerator_size) do |yielder, *args|
        val = args.length >= 2 ? args : args.first
        rests = lists.map { |list|
                  case list
                  when Array
                    list[index]
                  else
                    begin
                      list.next
                    rescue StopIteration
                      nil
                    end
                  end
                }
        yielder.yield [val, *rests]
        index += 1
      end
    end
  end

  class Iterator
    STACK_SIZE = 1_048_576

    attr_reader :result

    def initialize(obj)
      @object = obj
      rewind
    end

    private :initialize

    def next?
      !@done
    end

    def next
      reset unless @fiber

      val = @fiber.resume

      raise StopIteration, "iteration has ended" if @done

      return val
    end

    def rewind
      @fiber = nil
      @done = false
    end

    def reset
      @done = false
      @fiber = Fiber.new stack_size: STACK_SIZE do
        obj = @object
        @result = obj.each { |*val| Fiber.yield *val }
        @done = true
      end
    end
  end
end
