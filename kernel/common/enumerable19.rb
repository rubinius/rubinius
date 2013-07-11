# -*- encoding: us-ascii -*-

module Enumerable
  def chunk(initial_state = nil, &original_block)
    raise ArgumentError, "no block given" unless block_given?
    ::Enumerator.new do |yielder|
      previous = nil
      accumulate = []
      block = if initial_state.nil?
        original_block
      else
        duplicated_initial_state = initial_state.dup
        Proc.new{ |val| original_block.yield(val, duplicated_initial_state)}
      end
      each do |val|
        key = block.yield(val)
        if key.nil? || (key.is_a?(Symbol) && key.to_s[0, 1] == "_")
          yielder.yield [previous, accumulate] unless accumulate.empty?
          accumulate = []
          previous = nil
          case key
          when nil, :_separator
          when :_alone
            yielder.yield [key, [val]]
          else
            raise RuntimeError, "symbols beginning with an underscore are reserved"
          end
        else
          if previous.nil? || previous == key
            accumulate << val
          else
            yielder.yield [previous, accumulate] unless accumulate.empty?
            accumulate = [val]
          end
          previous = key
        end
      end
      yielder.yield [previous, accumulate] unless accumulate.empty?
    end
  end

  def collect
    if block_given?
      ary = []
      each do |*o|
        ary << yield(*o)
      end
      ary
    else
      to_enum :collect
    end
  end

  alias_method :map, :collect

  def each_entry(*pass)
    return to_enum :each_entry, *pass unless block_given?
    each(*pass) do |*args|
      yield args.size == 1 ? args[0] : args
    end
    self
  end

  def each_with_object(memo)
    return to_enum :each_with_object, memo unless block_given?
    each do
      obj = Rubinius.single_block_arg
      yield obj, memo
    end
    memo
  end

  alias_method :with_object, :each_with_object

  def flat_map(&block)
    return to_enum(:flat_map) unless block_given?
    map(&block).flatten(1)
  end
  alias_method :collect_concat, :flat_map

  def slice_before(arg = undefined, &block)
    if block_given?
      has_init = !(undefined.equal? arg)
    else
      raise ArgumentError, "wrong number of arguments (0 for 1)" if undefined.equal? arg
      block = Proc.new{ |elem| arg === elem }
    end
    Enumerator.new do |yielder|
      init = arg.dup if has_init
      accumulator = nil
      each do |elem|
        start_new = has_init ? block.yield(elem, init) : block.yield(elem)
        if start_new
          yielder.yield accumulator if accumulator
          accumulator = [elem]
        else
          accumulator ||= []
          accumulator << elem
        end
      end
      yielder.yield accumulator if accumulator
    end
  end

  def zip(*args)
    args.map! do |a|
      if a.respond_to? :to_ary
        a.to_ary
      else
        a.to_enum(:each)
      end
    end

    results = []
    i = 0
    each do
      o = Rubinius.single_block_arg
      entry = args.inject([o]) do |ary, a|
        ary << case a
               when Array
                 a[i]
               else
                 begin
                   a.next
                 rescue StopIteration
                   nil
                 end
               end
      end

      yield entry if block_given?

      results << entry
      i += 1
    end

    return nil if block_given?
    results
  end

  def each_with_index(*args)
    return to_enum(:each_with_index, *args) unless block_given?

    idx = 0
    each(*args) do
      o = Rubinius.single_block_arg
      yield o, idx
      idx += 1
    end

    self
  end

end
