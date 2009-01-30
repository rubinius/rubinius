# TODO: document TestGenerator
class TestGenerator
  class Label
    attr_reader :ip

    def initialize(gen)
      @generator = gen
      @ip = nil
    end

    def inspect
      self.to_sym.inspect
    end

    def to_sym
      raise "Unset label!" unless @ip
      :"label_#{@ip}"
    end

    def set!
      @ip = @generator.new_label_id
      @generator.set_label self.to_sym
    end

    def ==(lbl)
      raise "Unset label!" unless @ip
      return self.to_sym == lbl if Symbol === lbl
      @ip == lbl.ip
    end
  end

  class ExceptionBlock
    attr_accessor :start, :fin, :handler

    def initialize(g)
      @generator = g
      @start = g.new_label
      @handler = g.new_label
      g.setup_unwind @handler
    end

    def handle!
      @handler.set!
    end

    def escape(label)
      @generator.pop_unwind
      @generator.goto label
    end
  end

  # TestGenerator methods

  opcodes  = InstructionSet::OpCodes.map { |desc| desc.opcode }
  opcodes += [:add_literal,
              :gif,
              :git,
              :pop_modifiers,
              :push,
              :push_literal_at,
              :push_modifiers,
              :push_unique_literal,
              :send,
              :send_super,
              :send_with_block,
              :send_with_splat,
              :swap]
  opcodes -= [:class,
              :goto,
              :set_label]

  opcodes.each do |name|
    class_eval <<-CODE
      def #{name}(*args)
        add :#{name}, *args
      end
    CODE
  end

  # The :g accessor is provided to make the code in the
  # utility methods below clearer
  attr_accessor :stream, :ip, :redo, :break, :next, :retry
  attr_reader :g, :file, :line

  def initialize
    @stream = []
    @ip     = 0
    @lbl    = 0
    @slot   = 0
    @g      = self
  end

  def run(node)
    node.bytecode(self)
  end

  def convert_to_ary(ary)
    ary.map do |item|
      if item.respond_to? :to_ary
        convert_to_ary item.to_ary
      else
        item
      end
    end
  end

  def to_ary
    convert_to_ary [:test_generator, @stream]
  end

  def pretty_inspect
    to_ary.pretty_inspect
  end

  def inspect
    to_ary.inspect
  end

  def add(*args)
    @stream << args
    @ip += 1
  end

  def new_slot
    @ip
  end

  def new_label
    Label.new self
  end

  def new_label_id
    @lbl += 1
    @lbl
  end

  def exceptions
    eb = ExceptionBlock.new self
    eb.start.set!
    yield eb
  end

  def dup
    add :dup
  end

  def set_line(line, file)
    @file, @line = file, line
  end

  def set_label(lbl)
    @stream << [:set_label, lbl.to_sym]
  end

  def close
  end

  def advanced_since?(ip)
    ip < @ip
  end

  def ==(tg)
    tg.class == self.class && tg.stream == @stream
  end

  # Hack to provide expected semantics
  def ===(other)
    equal? other
  end

  def goto x
    raise "Bad goto: #{x.inspect} on #{caller.first}" unless Label === x
    add :goto, x
  end

  def lvar_set slot
    g.shift_array
    g.set_local slot
    g.pop
  end

  def lvar_at slot
    g.shift_array
    g.set_local_depth 0, slot
    g.pop
  end

  def as_primitive(name)
    @primitive = name
  end

  def find_cpath_top_const(name)
    g.push_cpath_top
    g.find_const name
  end

  def return_raise
    g.push_cpath_top
    g.find_const :LongReturnException
    g.swap
    g.push_context
    g.swap
    g.send :directed_to, 2
    g.raise_exc
  end

  def break_raise
    g.push_cpath_top
    g.find_const :BlockBreakException
    g.swap
    g.push_context
    g.swap
    g.send :directed_to, 2
    g.raise_exc
  end

  def return_rescue
    ok = g.new_label
    g.exceptions do |ex|
      yield
      ex.escape ok

      ex.handle!

      g.push_exception
      g.dup
      g.push_cpath_top
      g.find_const :LongReturnException
      g.swap
      g.kind_of

      reraise = g.new_label
      g.gif reraise

      # Test if this LRE is for us
      g.dup
      g.send :destination, 0
      g.push_context
      g.equal

      g.gif reraise

      # Ok, this is for us!
      g.clear_exception

      g.send :value, 0
      g.ret

      reraise.set!

      g.raise_exc
    end

    ok.set!
  end

  def break_rescue
    ok = g.new_label
    g.exceptions do |ex|
      yield
      ex.escape ok

      ex.handle!

      g.push_exception
      g.dup
      g.push_cpath_top
      g.find_const :BlockBreakException
      g.swap
      g.kind_of

      reraise = g.new_label
      g.gif reraise

      # Test if this LRE is for us
      g.dup
      g.send :destination, 0
      g.push_context
      g.equal

      g.gif reraise

      # Ok, this is for us!
      g.clear_exception

      # We leave the value on the stack as the return value
      g.send :value, 0

      done = g.new_label
      g.goto done

      reraise.set!

      g.raise_exc

      done.set!
    end

    ok.set!
  end

  def passed_block(local=0, in_block=false)
    if in_block
      g.break_rescue do
        yield
      end
    else
      g.return_rescue do
        g.break_rescue do
          yield
        end
      end
    end
  end

  def push_literal_desc(name = nil)
    desc = description name do |d|
      yield d
    end

    g.push_literal desc
  end

  def open_class(name)
    g.add :open_class, name
    g.dup
    g.send :opened_class, 0
    g.pop
  end

  def open_class_under(name)
    g.add :open_class_under, name
    g.dup
    g.send :opened_class, 0
    g.pop
  end

  def description name = nil
    desc = Compiler::MethodDescription.new TestGenerator, 0
    desc.name = name if name

    yield desc.generator

    desc
  end

  def block_description
    iter = description :__block__ do |d|
      yield d
    end
    iter.required = -1

    g.create_block iter

    iter
  end

  def array_of_splatted_array(n=1)
    g.make_array 0

    if block_given?
      yield
    else
      g.push 1
    end
    g.make_array n
    g.cast_array

    g.send :+, 1
  end

  def splatted_array(n=1)
    bottom = g.new_label

    if block_given?
      yield
    else
      g.push 1
    end
    g.make_array n

    g.cast_array
    g.dup
    g.send :size, 0
    g.push 1
    g.send :>, 1
    g.git bottom

    g.push 0
    g.send :at, 1

    bottom.set!
  end

  def save_exception(index = 0)
    g.push_exception
    g.set_local index
    g.pop
  end

  def restore_exception(index = 0)
    g.push_local index
    g.pop_exception
  end

  def in_block_send(name, type, required=nil, call_count=0, vis=true)
    iter = block_description do |d|
      count = nil

      case type
      when :none
        required = -1
      when :empty
        required = 0
      when :blank
        required = -1
        count = 0
      when :single
        required = 1
        d.cast_for_single_block_arg
        d.set_local_depth 0, 0
      when :splat
        required = -1
        d.cast_array
        d.cast_array
        d.set_local_depth 0, 0
      when :rest
        count = required.abs - 1
      when :multi
        count = required.abs
      end

      if count
        d.cast_for_multi_block_arg
        d.cast_array

        (0...count).each do |n|
          d.shift_array
          d.set_local_depth 0, n
          d.pop
        end
      end

      if type == :rest
        d.cast_array
        d.set_local_depth 0, count
      end

      d.pop
      d.push_modifiers
      d.new_label.set!

      yield d

      d.pop_modifiers
      d.ret
    end
    iter.required = required

    g.send_with_block name, call_count, vis
  end

  def in_class name
    case name
    when Symbol then
      g.push :nil
      g.open_class name
    when String then
      levels = name.split(/::/).map { |s| s.to_sym }
      klass = levels.pop

      levels.each do |level|
        g.push_const level
      end

      g.push :nil
      g.open_class_under klass
    end

    g.dup
    g.push_literal_desc do |d|
      d.push_self # FIX
      d.add_scope

      yield d

      d.ret
    end
    g.swap
    g.attach_method :__class_init__
    g.pop
    g.send :__class_init__, 0
  end

  def in_method name, singleton=false
    if singleton
      g.send :metaclass, 0
    else
      g.push_context
    end

    g.push_literal name
    g.push_literal_desc name do |d|
      yield d
      d.ret
    end

    if singleton then
      g.send :attach_method, 2
    else
      g.send :__add_method__, 2
    end
  end

  def in_module name
    case name
    when Symbol then
      g.open_module name
    when String then
      levels = name.split(/::/).map { |s| s.to_sym }
      klass = levels.pop

      levels.each do |level|
        g.push_const level
      end

      g.open_module_under klass
    end

    g.dup
    g.push_literal_desc do |d|
      d.push_self # FIX
      d.add_scope

      yield d

      d.ret
    end
    g.swap
    g.attach_method :__module_init__
    g.pop
    g.send :__module_init__, 0
  end

  def in_rescue(*klasses)
    g.push_modifiers

    jump_retry   = g.new_label
    jump_else    = g.new_label
    jump_last    = g.new_label

    has_ensure = klasses.delete :ensure
    saved_exception_index = klasses.detect { |a| a.instance_of?(Fixnum) }
    if saved_exception_index
      klasses.delete saved_exception_index
    else
      saved_exception_index = 0
    end

    g.save_exception saved_exception_index

    jump_retry.set!
    exceptions do |ex|

      yield :body

      ex.escape jump_else

      ex.handle!

      klasses.flatten.each do |klass|
        jump_body = self.new_label
        jump_next = self.new_label

        self.push_const klass
        self.push_exception
        self.send :===, 1
        self.git jump_body

        self.goto jump_next         # FIX: stupid jump, gifucked better

        jump_body.set!

        yield klass

        self.clear_exception
        self.goto jump_last

        jump_next.set!
      end

      g.push_exception
      g.raise_exc
    end

    jump_else.set!

    yield :else

    jump_last.set!
    g.restore_exception saved_exception_index
    g.pop_modifiers

    if has_ensure then
      ensure_good = g.new_label
      ensure_bad = g.new_label # TODO: magic jump!

      g.goto ensure_good
      ensure_bad.set!
      yield :ensure
      g.push_exception
      g.raise_exc

      ensure_good.set!
      yield :ensure
    end
  end

  def optional_arg slot
    if_set = g.new_label
    g.passed_arg slot
    g.git if_set
    g.push 42
    g.set_local slot
    g.pop
    if_set.set!
  end

  def block_arg slot
    is_nil = g.new_label
    g.push_block
    g.dup
    g.is_nil
    g.git is_nil
    g.push_const :Proc
    g.swap
    g.send :__from_block__, 1
    is_nil.set!
    g.set_local slot
    g.pop
  end

  def memoize
    memo = g.new_label
    g.add_literal nil
    slot = new_slot
    g.push_literal_at slot
    g.dup
    g.is_nil
    g.gif memo
    g.pop

    yield g

    g.set_literal slot
    memo.set!
  end

  def undef_bytecode(*names)
    last_name = names.last
    names.each do |name|
      g.push_context
      g.push_literal name
      g.send :__undef_method__, 1
      g.pop unless name == last_name
    end
  end

  def invalid_context(name)
    g.push :self
    g.push_const :LocalJumpError
    g.push_literal "#{name} used in invalid context"
    g.send :raise, 2, true
  end
end
