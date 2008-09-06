require File.dirname(__FILE__) + '/../spec_helper'

$: << 'lib' # HACK

require File.dirname(__FILE__) + '/../../lib/compiler/compiler'
require File.dirname(__FILE__) + '/../../lib/compiler/generator'
require File.dirname(__FILE__) + '/../../lib/compiler/bytecode'
require File.dirname(__FILE__) + '/../../lib/compiler/text'
require File.dirname(__FILE__) + '/../../kernel/delta/iseq'

######################################################################
## Added for ruby_parser transition... all of this should be temporary
## HACK:

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
    [:test_generator, @stream].inspect
  end

# TODO:
#   def pretty_print(pp)
#     pp.group(1, '[:test_generator, ', ']') {
#       pp.seplist(@stream) {|v|
#         pp.pp v
#       }
#     }
#   end

#   def pretty_print_cycle(pp)
#     pp.text(empty? ? '[xxx]' : '[xxx...]')
#   end

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
    @stream << [:set_label, lbl]
  end

  attr_accessor :redo, :break, :next, :retry, :ensure_return
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

  (opcodes + stupids - [:class]).each do |name|
    class_eval <<-CODE
      def #{name}(*args)
        add :#{name}, *args
      end
    CODE
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

  # Emits userland style code only.
  def add_method(name)
    self.push_literal name
    self.push_context
    self.send :__add_method__, 2
  end

  def push_self_or_class
    lbl = self.new_label
    self.push_cpath_top
    self.find_const :Module
    self.push :self
    self.kind_of
    self.git lbl
    self.send :class, 0
    lbl.set!
  end

  def push_literal_desc
    desc = description do |d|
      yield d
    end

    self.push_literal desc
  end

  def in_class name
    self.push :nil
    self.open_class name
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
end

def gen(sexp, plugins=[])
  @comp = Compiler.new TestGenerator
  plugins.each { |n| @comp.activate n }
  expected = TestGenerator.new

  yield expected

  @node = @comp.convert_sexp [:snippit, sexp]
  actual = TestGenerator.new
  @node.bytecode actual
  actual.should == expected

  @comp
end

def gen_iter x
  gen x do |g|
    desc = description do |d|

      yield d

      d.pop
      d.push_modifiers
      d.new_label.set!
      d.push :nil
      d.pop_modifiers
      d.ret
    end

    g.push :self
    g.send :ary, 0, true
    g.create_block desc

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
