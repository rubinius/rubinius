module Enumerable
  ##
  # :call-seq:
  #    enum.chunk {|elt| ... } => enumerator
  #    enum.chunk(initial_state) {|elt, state| ... } => enumerator
  #
  # Creates an enumerator for each chunked elements.
  # The consecutive elements which have same block value are chunked.
  #
  # The result enumerator yields the block value and an array of chunked elements.
  # So "each" method can be called as follows.
  #
  #   enum.chunk {|elt| key }.each {|key, ary| ... }
  #   enum.chunk(initial_state) {|elt, state| key }.each {|key, ary| ... }
  #
  # For example, consecutive even numbers and odd numbers can be
  # splitted as follows.
  #
  #   [3,1,4,1,5,9,2,6,5,3,5].chunk {|n|
  #     n.even?
  #   }.each {|even, ary|
  #     p [even, ary]
  #   }
  #   #=> [false, [3, 1]]
  #   #   [true, [4]]
  #   #   [false, [1, 5, 9]]
  #   #   [true, [2, 6]]
  #   #   [false, [5, 3, 5]]
  #
  # This method is especially useful for sorted series of elements.
  # The following example counts words for each initial letter.
  #
  #   open("/usr/share/dict/words", "r:iso-8859-1") {|f|
  #     f.chunk {|line| line.ord }.each {|ch, lines| p [ch.chr, lines.length] }
  #   }
  #   #=> ["\n", 1]
  #   #   ["A", 1327]
  #   #   ["B", 1372]
  #   #   ["C", 1507]
  #   #   ["D", 791]
  #   #   ...
  #
  # The following key values has special meaning:
  # - nil and :_separator specifies that the elements are dropped.
  # - :_alone specifies that the element should be chunked as a singleton.
  # Other symbols which begins an underscore are reserved.
  #
  # nil and :_separator can be used to ignore some elements.
  # For example, the sequence of hyphens in svn log can be eliminated as follows.
  #
  #   sep = "-"*72 + "\n"
  #   IO.popen("svn log README") {|f|
  #     f.chunk {|line|
  #       line != sep || nil
  #     }.each {|_, lines|
  #       pp lines
  #     }
  #   }
  #   #=> ["r20018 | knu | 2008-10-29 13:20:42 +0900 (Wed, 29 Oct 2008) | 2 lines\n",
  #   #    "\n",
  #   #    "* README, README.ja: Update the portability section.\n",
  #   #    "\n"]
  #   #   ["r16725 | knu | 2008-05-31 23:34:23 +0900 (Sat, 31 May 2008) | 2 lines\n",
  #   #    "\n",
  #   #    "* README, README.ja: Add a note about default C flags.\n",
  #   #    "\n"]
  #   #   ...
  #
  # paragraphs separated by empty lines can be parsed as follows.
  #
  #   File.foreach("README").chunk {|line|
  #     /\A\s*\z/ !~ line || nil
  #   }.each {|_, lines|
  #     pp lines
  #   }
  #
  # :_alone can be used to pass through bunch of elements.
  # For example, sort consecutive lines formed as Foo#bar and
  # pass other lines, chunk can be used as follows.
  #
  #   pat = /\A[A-Z][A-Za-z0-9_]+\#/
  #   open(filename) {|f|
  #     f.chunk {|line| pat =~ line ? $& : :_alone }.each {|key, lines|
  #       if key != :_alone
  #         print lines.sort.join('')
  #       else
  #         print lines.join('')
  #       end
  #     }
  #   }
  #
  # If the block needs to maintain state over multiple elements,
  # _initial_state_ argument can be used.
  # If non-nil value is given,
  # it is duplicated for each "each" method invocation of the enumerator.
  # The duplicated object is passed to 2nd argument of the block for "chunk" method.

  def chunk(initial_state = nil, &original_block)
    raise ArgumentError, "no block given" unless block_given?
    ::Enumerator.new do |yielder|
      previous = nil
      accumulate = []
      block = initial_state.nil? ? original_block : Proc.new{|val| original_block.yield(val, initial_state.clone)}
      each do |val|
        key = block.yield(val)
        if key.nil? || (key.is_a?(Symbol) && key.to_s[0,1] == "_")
          yielder.yield [previous, accumulate] unless accumulate.empty?
          accumulate = []
          previous = nil
          case key
          when nil, :_separator
          when :_singleton
            yielder.yield [key, [val]]
          else
            raise RuntimeError, "symbol beginning with an underscore are reserved"
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

  ##
  # :call-seq:
  #   enum.collect { | obj | block }  => array
  #   enum.map     { | obj | block }  => array
  #
  # Returns a new array with the results of running +block+ once for every
  # element in +enum+.
  #
  #   (1..4).collect { |i| i*i }   #=> [1, 4, 9, 16]
  #   (1..4).collect { "cat"  }   #=> ["cat", "cat", "cat", "cat"]

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

  #
  # call-seq:
  #    enum.each_entry {|obj| block}  => enum
  #
  # Calls <i>block</i> once for each element in <i>self</i>, passing that
  # element as a parameter, converting multiple values from yield to an
  # array.
  #
  #    class Foo
  #      include Enumerable
  #      def each
  #        yield 1
  #        yield 1,2
  #      end
  #    end
  #    Foo.new.each_entry{|o| print o, " -- "}
  #
  # produces:
  #
  #    1 -- [1, 2] --
  #
  def each_entry(*pass)
    return to_enum :each_entry, *pass unless block_given?
    each(*pass) do |*args|
      yield args.size == 1 ? args[0] : args
    end
    self
  end

  ##
  # :call-seq:
  #   each_with_object(obj) {|(*args), memo_obj| ... }
  #   each_with_object(obj)
  #
  # Iterates the given block for each element with an arbitrary
  # object given, and returns the initially given object.
  #
  # If no block is given, returns an enumerator.
  #
  # e.g.:
  #     evens = (1..10).each_with_object([]) {|i, a| a << i*2 }
  #     # => [2, 4, 6, 8, 10, 12, 14, 16, 18, 20]

  def each_with_object(memo)
    return to_enum :each_with_object, memo unless block_given?
    each {|obj| yield obj, memo}
    memo
  end

  alias_method :with_object, :each_with_object

  ##
  # :call-seq:
  #    enum.flat_map       {| obj | block }  => array
  #    enum.collect_concat {| obj | block }  => array
  #
  # Returns a new array with the concatenated results of running
  # <em>block</em> once for every element in <i>enum</i>.
  #
  #    [[1,2],[3,4]].flat_map {|i| i }   #=> [1, 2, 3, 4]

  def flat_map(&block)
    return to_enum(:flat_map) unless block_given?
    map(&block).flatten(1)
  end
  alias_method :collect_concat, :flat_map

  #
  #  call-seq:
  #     enum.slice_before(pattern) => enumerator
  #     enum.slice_before {|elt| bool } => enumerator
  #     enum.slice_before(initial_state) {|elt, state| bool } => enumerator
  #
  #  Creates an enumerator for each chunked elements.
  #  The beginnings of chunks are defined by _pattern_ and the block.
  #  If _pattern_ === _elt_ returns true or 
  #  the block returns true for the element,
  #  the element is beginning of a chunk.
  #
  #  The result enumerator yields the chunked elements as an array.
  #  So "each" method can be called as follows.
  #
  #    enum.slice_before(pattern).each {|ary| ... }
  #    enum.slice_before {|elt| bool }.each {|ary| ... }
  #    enum.slice_before(initial_state) {|elt, state| bool }.each {|ary| ... }
  #
  #  For example, iteration over ChangeLog entries can be implemented as
  #  follows.
  #
  #    # iterate over ChangeLog entries.
  #    open("ChangeLog") {|f|
  #      f.slice_before(/\A\S/).each {|e| pp e}
  #    }
  #
  #    # same as above.  block is used instead of pattern argument.
  #    open("ChangeLog") {|f|
  #      f.slice_before {|line| /\A\S/ === line }.each {|e| pp e}
  #    }
  #
  # "svn proplist -R" produces multiline output for each file.
  # They can be chunked as follows: 
  #
  #    IO.popen([{"LC_ALL"=>"C"}, "svn", "proplist", "-R"]) {|f|
  #      f.lines.slice_before(/\AProp/).each {|lines| p lines }
  #    }
  #    #=> ["Properties on '.':\n", "  svn:ignore\n", "  svk:merge\n"]
  #    #   ["Properties on 'goruby.c':\n", "  svn:eol-style\n"]
  #    #   ["Properties on 'complex.c':\n", "  svn:mime-type\n", "  svn:eol-style\n"]
  #    #   ["Properties on 'regparse.c':\n", "  svn:eol-style\n"]
  #    #   ...
  #
  #  If the block needs to maintain state over multiple elements,
  #  local variables can be used.
  #  For example, monotonically increasing elements can be chunked as follows.
  #
  #    a = [3,1,4,1,5,9,2,6,5,3,5]
  #    n = 0
  #    p a.slice_before {|elt|
  #      prev, n = n, elt
  #      prev > elt
  #    }.to_a
  #    #=> [[3], [1, 4], [1, 5, 9], [2, 6], [5], [3, 5]]
  #
  #  However local variables are not appropriate to maintain state
  #  if the result enumerator is used twice or more.
  #  In such case, the last state of the 1st +each+ is used in 2nd +each+.
  #  _initial_state_ argument can be used to avoid this problem.
  #  If non-nil value is given as _initial_state_,
  #  it is duplicated for each "each" method invocation of the enumerator.
  #  The duplicated object is passed to 2nd argument of the block for
  #  +slice_before+ method.
  #
  #    # word wrapping.
  #    # this assumes all characters have same width.
  #    def wordwrap(words, maxwidth)
  #      # if cols is a local variable, 2nd "each" may start with non-zero cols.
  #      words.slice_before(cols: 0) {|w, h|
  #        h[:cols] += 1 if h[:cols] != 0
  #        h[:cols] += w.length
  #        if maxwidth < h[:cols]
  #          h[:cols] = w.length
  #          true
  #        else
  #          false
  #        end
  #      }
  #    end
  #    text = (1..20).to_a.join(" ")
  #    enum = wordwrap(text.split(/\s+/), 10)
  #    puts "-"*10
  #    enum.each {|ws| puts ws.join(" ") }
  #    puts "-"*10
  #    #=> ----------
  #    #   1 2 3 4 5
  #    #   6 7 8 9 10
  #    #   11 12 13
  #    #   14 15 16
  #    #   17 18 19
  #    #   20
  #    #   ----------
  #
  # mbox contains series of mails which start with Unix From line.
  # So each mail can be extracted by slice before Unix From line.
  #
  #    # parse mbox
  #    open("mbox") {|f|
  #      f.slice_before {|line|
  #        line.start_with? "From "
  #      }.each {|mail|
  #        unix_from = mail.shift
  #        i = mail.index("\n")
  #        header = mail[0...i]
  #        body = mail[(i+1)..-1]
  #        fields = header.slice_before {|line| !" \t".include?(line[0]) }.to_a
  #        p unix_from
  #        pp fields
  #        pp body
  #      }
  #    }
  #
  #    # split mails in mbox (slice before Unix From line after an empty line)
  #    open("mbox") {|f|
  #      f.slice_before(emp: true) {|line,h|
  #      prevemp = h[:emp]
  #      h[:emp] = line == "\n"
  #      prevemp && line.start_with?("From ")
  #    }.each {|mail|
  #      pp mail
  #    }
  #
  #
  def slice_before(arg = undefined, &block)
    if block_given?
      has_init = !(arg.equal? undefined)
    else
      raise ArgumentError, "wrong number of arguments (0 for 1)" if arg.equal? undefined
      block = Proc.new{|elem| arg === elem }
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

  ##
  # :call-seq:
  #    enum.zip(arg, ...)                   => array
  #    enum.zip(arg, ...) { |arr| block }   => nil
  #
  # Converts any arguments by using +#to_ary+, or +#each+, then
  # merges elements of +enum+ with corresponding elements from each
  # argument. This generates a sequence of enum#size +n+-element
  # arrays, where +n+ is one more that the count of arguments. If the
  # size of any argument is less than enum#size, nil values are supplied.
  # If a block given, it is invoked for each output array, otherwise
  # an array of arrays is returned.
  #
  #   a = [ 4, 5, 6 ]
  #   b = [ 7, 8, 9 ]
  #
  #   (1..3).zip(a, b)      #=> [[1, 4, 7], [2, 5, 8], [3, 6, 9]]
  #   "cat\ndog".zip([1])   #=> [["cat\n", 1], ["dog", nil]]
  #   (1..3).zip            #=> [[1], [2], [3]]
  #
  def zip(*args)
    args.map! do |a|
      if a.respond_to? :to_ary
        a.to_ary
      else
        a.to_enum(:each)
      end
    end

    results = []

    each_with_index do |o, i|
      entry = args.inject([o]) do |ary, a|
        case a
        when Array
          ary << a[i]
        else
          ary << a.next
        end
      end

      yield entry if block_given?

      results << entry
    end

    return nil if block_given?
    results
  end

  ##
  # :call-seq:
  #   enum.each_with_index { |obj, i| block }  -> enum or enumerator
  #
  # Calls +block+ with two arguments, the item and its index, for
  # each item in +enum+. Given arguments are passed through to
  # +#each+.
  #
  #   hash = {}
  #   %w[cat dog wombat].each_with_index { |item, index|
  #     hash[item] = index
  #   }
  #
  #   p hash   #=> {"cat"=>0, "wombat"=>2, "dog"=>1}
  def each_with_index(*args)
    return to_enum(:each_with_index, *args) unless block_given?

    idx = 0
    each(*args) do |o|
      yield o, idx
      idx += 1
    end

    self
  end

end
