class Fixnum
  # evade Rational
  alias_method :divide, :/
end

# Use aliases to keep our actions from polluting the
# data collection efforts.
class Hash
  alias_method :class_spy_get,   :[]
  alias_method :class_spy_set,   :[]=
  alias_method :class_spy_each,  :each
  alias_method :class_spy_keys,  :keys
  alias_method :class_spy_size,  :size
  alias_method :class_spy_sort,  :sort
  alias_method :class_spy_store, :store
end

class Array
  alias_method :class_spy_append, :<<
  alias_method :class_spy_get,    :[]
  alias_method :class_spy_set,    :[]=
  alias_method :class_spy_each,   :each
  alias_method :class_spy_inject, :inject
  alias_method :class_spy_map,    :map
  alias_method :class_spy_size,   :size
end

class ClassSpy
  attr_reader :data, :objects

  def self.spies
    @spies ||= {}
  end

  def self.register(spy)
    spies.class_spy_set spy.object_id, spy
  end

  def self.get(id)
    spies.class_spy_get id
  end

  def initialize(klass, methods)
    @klass = klass
    if klass.method_defined? :size
      klass.send :alias_method, :class_spy_size, :size
      @size = Stat.new
    end
    @methods = methods.class_spy_map { |sym| [sym, Stat.new] }
    @data = Hash.new { |h, k| h.class_spy_store(k, data_entry) }
    @objects = {}

    self.class.register self
    install_method_spies

    at_exit { self.show }
  end

  def install_method_spies
    @installing_method_spies = true
    @methods.class_spy_each do |name, stat|
      spy_name = "class_spy_#{map_name name}"
      @klass.module_eval <<-CODE
        alias_method :#{spy_name}, #{name.inspect}
        def #{name}(*args, &block)
          ClassSpy.get(#{object_id}).called(self, #{name.inspect})
          #{spy_name}(*args, &block)
        end
      CODE
    end
    @installing_method_spies = false
  end

  NAME_MAP = {
    :'`'   => 'backtick',
    :'+'   => 'plus',
    :'-'   => 'minus',
    :'+@'  => 'uplus',
    :'-@'  => 'uminus',
    :'*'   => 'multiply',
    :'/'   => 'divide',
    :'%'   => 'modulo',
    :'<<'  => 'chevron_left',
    :'>>'  => 'chevron_right',
    :'<'   => 'lt',
    :'<='  => 'lte',
    :'>'   => 'gt',
    :'>='  => 'gte',
    :'='   => 'assignment',
    :'=='  => 'equal_value',
    :'===' => 'case_compare',
    :'<=>' => 'comparison',
    :'[]'  => 'element_reference',
    :'[]=' => 'element_set',
    :'**'  => 'exponent',
    :'!'   => 'not',
    :'~'   => 'tilde',
    :'!='  => 'not_equal',
    :'!~'  => 'not_match',
    :'=~'  => 'match',
    :'&'   => 'ampersand',
    :'|'   => 'bar',
    :'^'   => 'hat'
  }

  def map_name(name)
    if mapped = NAME_MAP.class_spy_get(name)
      mapped
    else
      name
    end
  end

  def store_objects
    @store_objects = true
  end

  def store_objects?
    @store_objects
  end

  def data_entry
    Hash.new { |h, k| h.class_spy_store(k, 0) }
  end

  def set(id, key, value)
    entry = data.class_spy_get(id)
    entry.class_spy_store(key, value)
  end

  def increment(entry, key)
    value = entry.class_spy_get(key)
    entry.class_spy_store(key, value + 1)
  end

  def called(obj, key)
    return if @installing_method_spies

    entry = data.class_spy_get(obj.object_id)
    increment entry, key

    # This is an extra bit of work for every method called
    # but it's faster than saving every object in order to
    # get the size when the results are printed. Orders of
    # magnitude faster when the number of objects inspected
    # is very large.
    entry.class_spy_store(:size, obj.class_spy_size) if @size

    objects.class_spy_store obj.object_id, obj if store_objects?
  end

  def each
    @data.class_spy_each do |id, entry|
      yield objects.class_spy_get(id), entry
    end
  end

  def each_object
    @objects.class_spy_each do |id, obj|
      yield id, obj
    end
  end

  def show
    print "\nClassSpy results for #{@klass}\n\n"

    @data_size = data.class_spy_size
    if @data_size < 6
      puts "HashSpy stats need at least 6 entries (recorded #{@data_size})"
      return
    end

    each do |obj, entry|
      @size.record obj, entry.class_spy_get(:size) if @size

      @methods.class_spy_each do |name, stat|
        stat.record obj, entry.class_spy_get(name)
      end
    end

    format = "%-15s%8d %3d %5.1f %4d %4d %4d %6d %6d %6d %6d %6d %6d\n"

    puts "Method name (*)     Max Min  Mean  25%  50%  75%  81.3%  87.5%  93.8%  96.3%  97.5%  98.8%"
    puts "------------------------------------------------------------------------------------------"

    if @size
      printf format, "size",
             @size.max, @size.min, @size.mean,
             @size.Q1, @size.Q2, @size.Q3,
             @size.AQ1, @size.AQ2, @size.AQ3,
             @size.BQ1, @size.BQ2, @size.BQ3
    end

    @methods.class_spy_each do |name, stat|
      printf format,
             name, stat.max, stat.min, stat.mean,
             stat.Q1, stat.Q2, stat.Q3,
             stat.AQ1, stat.AQ2, stat.AQ3,
             stat.BQ1, stat.BQ2, stat.BQ3
    end

    puts "\n(*) Units are # of calls except for 'size' which is the object's size."
    puts "    The values listed in the percentages columns tells the number of"
    puts "    data points <= to the listed value. E.g. if 8 is listed under 50%"
    puts "    in the row for #each, then 50% of the instances had <= 8 calls to"
    puts "    the #each method."

    puts "\n#{@data_size} #{@klass} instances\n"

    finish if respond_to? :finish
  end

  class Stat
    attr_reader :data, :max_object

    def initialize
      @data = []
      @max = 0
      @max_object = nil
    end

    def record(obj, value)
      @sorted = false
      if value > @max
        @max = value
        @max_object = obj
      end
      @data.class_spy_append value
    end

    # We implement a sorting algorithm here because spying on methods
    # produced some odd failures from the core library sort method.
    #
    # Uses Heap sort because of the potentially very large data sets,
    # which already puts a great deal of pressure on the GC, and
    # because the data is already significantly sorted because of its
    # distribution (i.e. long runs of the same value, usually 0).
    #
    # This algorithm compares favorably to the core library #qsort
    # method on arrays of tens of thousands of elements.
    #
    # Code based on http://en.wikipedia.org/wiki/Heapsort
    #
    def sort
      return if @sorted

      bottom = n - 1
      i = (bottom - 1).divide(2)
      while i >= 0
        sift_down i, bottom
        i -= 1
      end

      i = bottom
      while i > 0
        tmp = @data.class_spy_get(0)
        @data.class_spy_set(0, @data.class_spy_get(i))
        @data.class_spy_set(i, tmp)

        i -= 1
        sift_down 0, i
      end

      @sorted = true
    end

    def sift_down(root, bottom)
      while root * 2 + 1 <= bottom
        child = root * 2 + 1
        if child + 1 <= bottom and
            @data.class_spy_get(child) < @data.class_spy_get(child + 1)
          child += 1
        end

        a = @data.class_spy_get(root)
        b = @data.class_spy_get(child)
        if a < b
          @data.class_spy_set(root, b)
          @data.class_spy_set(child, a)
          root = child
        else
          return
        end
      end
    end

    def odd?(x)
      x & 1 == 1
    end

    def max
      sort
      @data.class_spy_get(-1)
    end

    def min
      sort
      @data.class_spy_get(0)
    end

    def mean
      @data.class_spy_inject(0) { |sum, x| sum + x }.to_f / @data.class_spy_size
    end

    # We want to get some insight into the distribution of potentially very
    # large sets of data. We use the idea of Quartiles. Essentially, dividing
    # the sorted data set into 4 equal parts and looking at three key values.
    # The values are Q1, Q2, and Q3 where these are the values below which
    # 25%, 50% and 75% of the values in the data set are found respectively.
    # Q2 is also known as the median and Q1 is the median of [0, Q2) and Q3 is
    # the median of (Q2, T] where 0 is the index of the first datum and T is
    # the index of the last.
    #
    # The methods Q1, Q2, Q3 below implement the concepts described above.
    # Emperically, in large data sets with very flat distribution, these three
    # do not give as much information as desired. The methods AQ1, AQ2, and
    # AQ3 provide the same information on the interval [Q3, T].
    #
    # Finally, the methods BQ1, BQ2, BQ3 are the quartiles of the top 95% of
    # values.
    #
    # The methods are implemented as an abstraction of taking the median of a
    # data set given a base index and a size.

    # The size of the total data set.
    def n
      @data.class_spy_size
    end

    # The index of the last element of the first half of the full data set.
    def s
      (n - 1).divide(2)
    end

    # The size of half of the full data set.
    def t
      odd?(n) ? s : s + 1
    end

    def Q1
      median 0, t
    end

    def Q2
      median 0, n
    end

    def Q3
      median s + 1, t
    end

    # The index of the first element of [Q3, T].
    def aq_base
      (s + 1) + t.divide(2)
    end

    # The size of [Q3, T].
    def n2
      n - aq_base
    end

    # The index of the last element of the first half of [Q3, T].
    def s2
      (n2 - 1).divide(2)
    end

    # The size of half of [Q3, T].
    def t2
      odd?(n2) ? s2 : s2 + 1
    end

    def AQ1
      median aq_base, t2
    end

    def AQ2
      median aq_base, n2
    end

    def AQ3
      median aq_base + s2 + 1, t2
    end

    # The index of the first element in the top 5% of the list of values.
    def bq_base
      (n * 0.95).to_i
    end

    # The size of the top 5% of the list of values.
    def n3
      n - bq_base
    end

    # The index of the last element of the first half of the top
    # 5% of the list of values.
    def s3
      (n3 - 1).divide(2)
    end

    # The size of half of the top 5% of the list of values.
    def t3
      odd?(n3) ? s3 : s3 + 1
    end

    def BQ1
      median bq_base, t3
    end

    def BQ2
      median bq_base, n3
    end

    def BQ3
      median bq_base + s3 + 1, t3
    end

    def median(base, size)
      sort

      a = @data.class_spy_get(base + size.divide(2))
      return a if odd? size

      b = @data.class_spy_get(base + (size - 1).divide(2))
      (a + b).divide(2)
    end
  end
end
