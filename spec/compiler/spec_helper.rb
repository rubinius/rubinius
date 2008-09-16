require File.dirname(__FILE__) + '/../spec_helper'

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
def parse ruby
  ruby.to_sexp("spec")
end

######################################################################

class TestGenerator
  def initialize
    @stream = []
    @ip = 0
  end

  attr_reader :stream, :ip

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
    raise "Bad set_label: #{lbl.inspect} on #{caller.first}" unless
      Label === lbl
    @stream << [:set_label, lbl]
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
      ":label_#{@ip}"
    end

    attr_reader :ip

    def set!
      @ip = @generator.ip
      @generator.set_label self
    end

    def ==(lbl)
      raise "Unset label!" unless @ip
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

  def passed_block(local=0, in_block=false)
    g = self
    ok = g.new_label
    g.exceptions do |ex|
      g.push_cpath_top
      g.find_const :LongReturnException
      g.send :allocate, 0
      g.set_local local
      g.pop

      yield
      g.goto ok

      ex.handle!

      g.push_exception
      g.dup
      g.push_local local
      g.equal

      after = g.new_label
      g.gif after

      g.clear_exception
      leave = g.new_label
      g.dup
      g.send :is_return, 0

      g.gif leave

      unless in_block
        g.send :value, 0
        g.ret
      end

      after.set!

      g.raise_exc

      leave.set!
      g.send :value, 0
    end

    ok.set!
  end

  def push_literal_desc
    desc = description do |d|
      yield d
    end

    self.push_literal desc
  end

  def in_rescue klass, wtf = false
    top       = self.new_label
    bottom    = self.new_label
    dunno     = self.new_label
    std_err   = self.new_label
    unhandled = self.new_label

    self.push_modifiers

    top.set!
    top.set!

    yield true

    self.goto bottom

    dunno.set!

    unless wtf then
      self.push_const klass
      self.push_exception
      self.send :===, 1
      self.git std_err
    end

    self.goto unhandled         # FIX: stupid jump, gif better

    std_err.set!

    yield false

    self.clear_exception
    self.goto bottom

    unhandled.set!

    self.push_exception
    self.raise_exc

    bottom.set!
    bottom.set!

    self.pop_modifiers
  end

  def in_block_send(msg,
                    block_count    = 0,
                    call_count     = 0,
                    block_send_vis = true,
                    shift          = 0,
                    nested         = false)

    self.create_block_desc do |d|
      inner_top = d.new_label

      case block_count
      when -1 then
        d.cast_array
        d.set_local_depth 0, 0
      when 0 then
      when 1 then
        d.cast_for_single_block_arg
        d.set_local_depth 0, 0
      else
        d.cast_for_multi_block_arg
        (0...block_count).each do |n|
          d.shift_tuple
          d.set_local_depth 0, n
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

    top      = self.new_label
    dunno1   = self.new_label
    dunno2   = self.new_label
    uncaught = self.new_label
    bottom   = self.new_label

    top.set!

    self.push_cpath_top
    self.find_const :LongReturnException
    self.send :allocate, 0
    self.set_local 0 + shift
    self.pop

    self.send_with_block msg, call_count, block_send_vis

    self.goto bottom

    dunno1.set!

    self.push_exception
    self.dup
    self.push_local 0 + shift
    self.equal
    self.gif uncaught
    self.clear_exception
    self.dup
    self.send :is_return, 0
    self.gif dunno2

    unless nested then
      self.send :value, 0
      self.ret
    end

    uncaught.set!
    self.raise_exc
    dunno2.set!
    self.send :value, 0
    bottom.set!
  end

  def create_block_desc
    desc = description do |d|
      yield d
    end

    self.create_block desc
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
    slot = @ip
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

  @comp
end

def gen_iter x
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

