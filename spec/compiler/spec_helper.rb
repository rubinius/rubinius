require File.dirname(__FILE__) + '/../spec_helper'

# Assume we can run the specs with rbx
unless defined?(RUBY_ENGINE) and RUBY_ENGINE == 'rbx'

$: << 'lib'

require File.dirname(__FILE__) + '/../../lib/compiler/compiler'
require File.dirname(__FILE__) + '/../../lib/compiler/generator'
require File.dirname(__FILE__) + '/../../lib/compiler/bytecode'
require File.dirname(__FILE__) + '/../../lib/compiler/text'
require File.dirname(__FILE__) + '/../../kernel/delta/iseq'

######################################################################
## Added for ruby_parser transition... all of this should be temporary
## HACK:

$: << 'lib/compiler'

require "sexp"
require "sexp_processor"

load "kernel/delta/string.rb"

require 'ruby_parser' # for s(...)

  # HACK HACK HACK
  def parse ruby
    ruby.to_sexp("spec")
  end
else
  # HACK HACK HACK
  def parse ruby
    Rubinius::LitRewriter.new.process ruby.to_sexp("spec")
  end
end   # unless defined?(RUBY_ENGINE)

######################################################################

class TestGenerator
  def initialize
    @stream = []
    @ip = 0
    @lbl = 0
    @slot = 0
  end

  def new_slot
    @ip
  end

  def new_label_id
    @lbl += 1
    @lbl
  end

  attr_accessor :stream, :ip

  def run(node)
    node.bytecode(self)
  end

  def inspect
    s(:test_generator, @stream).inspect
  end

  def add(*args)
    @stream << args
    @ip += 1
  end

  def dup
    add(:dup)
  end

  def set_line(line, file)
    @file, @line = file, line
  end

  def set_label(lbl)
#     raise "Bad set_label: #{lbl.inspect} on #{caller.first}" unless
#       Label === lbl
    @stream << [:set_label, lbl.to_sym]
  end

  attr_accessor :redo, :break, :next, :retry
  attr_reader :file, :line

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

  opcodes = InstructionSet::OpCodes.map { |desc| desc.opcode }
  stupids = [:add_literal, :gif, :git, :pop_modifiers, :push,
             :push_literal_at, :push_modifiers, :push_unique_literal, :send,
             :send_super, :send_with_block, :send_with_splat, :swap,]

  (opcodes + stupids - [:class, :goto, :set_label]).each do |name|
    class_eval <<-CODE
      def #{name}(*args)
        add :#{name}, *args
      end
    CODE
  end

  def goto x
    raise "Bad goto: #{x.inspect} on #{caller.first}" unless Label === x
    add :goto, x
  end

  class Label
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

    attr_reader :ip

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

  def new_label
    lbl = Label.new(self)
    return lbl
  end

  class ExceptionBlock
    def initialize(start, handler)
      @start = start
      @handler = handler
    end

    attr_accessor :start, :fin, :handler

    def handle!
      @handler.set!
    end
  end

  def exceptions
    eb = ExceptionBlock.new(self.new_label, self.new_label)
    eb.start.set!
    yield eb
  end

  def lvar_set slot
    shift_tuple
    set_local slot
    pop
  end

  def lvar_at slot
    shift_tuple
    set_local_depth 0, slot
    pop
  end

  def return_raise
    push_cpath_top
    find_const :LongReturnException
    swap
    push_context
    swap
    send :directed_to, 2
    raise_exc
  end

  def break_raise
    push_cpath_top
    find_const :BlockBreakException
    swap
    push_context
    swap
    send :directed_to, 2
    raise_exc

  end

  def return_rescue
    g = self
    ok = g.new_label
    g.exceptions do |ex|
      yield
      g.goto ok

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
    g = self
    ok = g.new_label
    g.exceptions do |ex|
      yield
      g.goto ok

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
      break_rescue do
        yield
      end
    else
      return_rescue do
        break_rescue do
          yield
        end
      end
    end
  end

  def push_literal_desc
    desc = description do |d|
      yield d
    end

    self.push_literal desc
  end

  def create_block_desc
    desc = description do |d|
      yield d
    end

    self.create_block desc
  end

  def in_block_send(msg,
                    block_count    = 0,
                    call_count     = 0,
                    block_send_vis = true,
                    shift          = 0,
                    nested         = false,
                    lvl            = 0)

    self.create_block_desc do |d|
      inner_top = d.new_label

      case block_count
      when Float then # yes... I'm a dick
        d.cast_for_single_block_arg
        d.set_local 0
      when Array then # I do, am a dick.
        d.cast_for_multi_block_arg
        (0...block_count[0]).each do |n|
          d.shift_tuple
          d.set_local_depth lvl, n
          d.pop
        end

        d.cast_array
        d.set_local_depth lvl, block_count[1]
      when -2 then
        d.cast_for_multi_block_arg
      when -1 then
        d.cast_array
        d.set_local_depth lvl, 0
      when 0 then
      when 1 then
        d.cast_for_single_block_arg
        d.set_local_depth lvl, 0
      else
        d.cast_for_multi_block_arg
        (0...block_count).each do |n|
          d.shift_tuple
          d.set_local_depth lvl, n
          d.pop
        end
      end

      d.pop

      d.push_modifiers
      inner_top.set!

      yield d

      d.pop_modifiers
      d.ret
    end

    if nested
      break_rescue do
        send_with_block msg, call_count, block_send_vis
      end
    else
      return_rescue do
        break_rescue do
          send_with_block msg, call_count, block_send_vis
        end
      end
    end
  end

  def in_class name
    case name
    when Symbol then
      self.push :nil
      self.open_class name
    when String then
      levels = name.split(/::/).map { |s| s.to_sym }
      klass = levels.pop

      levels.each do |level|
        self.push_const level
      end

      self.push :nil
      self.open_class_under klass
    end

    self.dup
    self.push_literal_desc do |d|
      d.push_self # FIX
      d.add_scope

      yield d

      d.ret
    end
    self.swap
    self.attach_method :__class_init__
    self.pop
    self.send :__class_init__, 0
  end

  def in_method name, singleton=false
    if singleton
      self.send :metaclass, 0
    else
      self.push_context
    end

    self.push_literal name
    self.push_literal_desc do |d|
      yield d
      d.ret
    end
    if singleton then
      self.send :attach_method, 2
    else
      self.send :__add_method__, 2
    end
  end

  def in_module name
    case name
    when Symbol then
      self.open_module name
    when String then
      levels = name.split(/::/).map { |s| s.to_sym }
      klass = levels.pop

      levels.each do |level|
        self.push_const level
      end

      self.open_module_under klass
    end

    self.dup
    self.push_literal_desc do |d|
      d.push_self # FIX
      d.add_scope

      yield d

      d.ret
    end
    self.swap
    self.attach_method :__module_init__
    self.pop
    self.send :__module_init__, 0
  end

  def in_rescue *klasses
    jump_top     = self.new_label
    jump_ex_body = self.new_label
    jump_else    = self.new_label
    jump_last    = self.new_label
    jump_matched = self.new_label

    has_ensure = klasses.delete :ensure

    self.push_modifiers

    jump_top.set!
    jump_ex_body.set!

    yield :body

    self.goto jump_else # TODO: g.goto_if_exception jump_ex1

    jump_matched.set! # TODO: magic jump! generate REAL bytecode!

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

    self.push_exception
    self.raise_exc

    jump_else.set!

    yield :else

    jump_last.set!
    self.pop_modifiers

    if has_ensure then
      ensure_good = self.new_label
      ensure_bad = self.new_label # TODO: magic jump!

      self.goto ensure_good
      ensure_bad.set!
      yield :ensure
      self.push_exception
      self.raise_exc

      ensure_good.set!
      yield :ensure
    end
  end

  def optional_arg slot
    if_set = self.new_label
    self.passed_arg slot
    self.git if_set
    self.push 42
    self.set_local slot
    self.pop
    if_set.set!
  end

  def block_arg slot
    is_nil = self.new_label
    self.push_block
    self.dup
    self.is_nil
    self.git is_nil
    self.push_const :Proc
    self.swap
    self.send :__from_block__, 1
    is_nil.set!
    self.set_local slot
    self.pop
  end

  def memoize
    memo = self.new_label
    self.add_literal nil
    slot = new_slot
    self.push_literal_at slot
    self.dup
    self.is_nil
    self.gif memo
    self.pop

    yield self

    self.set_literal slot
    memo.set!
  end

  def undef_bytecode(*names)
    last_name = names.last
    names.each do |name|
      self.push :self
      self.send :metaclass, 0
      self.push_literal name
      self.send :undef_method, 1
      self.pop unless name == last_name
    end
  end
end

def gen(sexp, plugins=[])
  @comp = Compiler.new TestGenerator
  plugins.each { |n| @comp.activate n }
  expected = TestGenerator.new

  yield expected

  sexp = Sexp.from_array sexp
  @node = @comp.convert_sexp s(:snippit, sexp)
  actual = TestGenerator.new
  @node.bytecode actual
  actual.should == expected

  body = @node.body
  if body.kind_of? Compiler::Node::Define
    @required, @optional, @splat = body.argument_info
  else
    @args = nil
  end

  @comp
end

def gen_iter x # TODO: fold this up into TestGenerator instead
  gen x do |g|
    g.push :self
    g.send :ary, 0, true

    g.create_block_desc do |d|
      yield d

      d.pop
      d.push_modifiers
      d.new_label.set!
      d.push :nil
      d.pop_modifiers
      d.ret
    end

    g.passed_block do
      g.send_with_block :each, 0, false
    end
  end
end

def description name = nil
  desc = Compiler::MethodDescription.new TestGenerator, 0
  desc.name = name if name
  yield desc.generator
  return desc
end

