require File.dirname(__FILE__) + '/../../spec/spec_helper'
$: << "compiler2"

require 'compiler'
require 'generate'
require 'bytecode'
require 'text'

class TestGenerator
  def initialize
    @stream = []
    @ip = 0
  end
  
  attr_reader :stream, :ip
  
  def inspect
    "#<TestGenerator #{@stream.inspect}"
  end
  
  def add(*args)
    @stream << args
    @ip += 1
  end
  
  def method_missing(*args)
    add *args
  end
  
  def set_line(line, file)
    @file, @line = file, line
  end
  
  def set_label(lbl)
    @stream << [:set_label, lbl]
  end
  
  def send(*stuff)
    method_missing :send, *stuff
  end
  
  def dup
    add :dup
  end
  
  def class
    raise "blah"
    add :class
  end
  
  attr_accessor :redo, :break, :next, :redo, :retry, :ensure_return
  attr_reader :file, :line
  
  def push_modifiers
  end
  
  def pop_modifiers
  end
  
  def close
  end
  
  def advanced_since?(ip)
    ip < @ip
  end
    
  def ==(tg)
    tg.stream == @stream
  end
  
  class Label
    def initialize(gen)
      @generator = gen
      @ip = nil
    end
    
    def inspect
      "#<Label #{ip}>"
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
    
    def handle
      @handler.set!
      yield
    end
  end
  
  def exceptions
    eb = ExceptionBlock.new(self.new_label, self.new_label)
    eb.start.set!
    yield eb
  end
  
end

def gen(sexp)
  comp = Compiler.new TestGenerator
  tg = TestGenerator.new
  
  yield tg
  
  node = comp.convert_sexp [:snippit, sexp]
  act = TestGenerator.new
  node.bytecode act
  act.should == tg
end

def description
  desc = Compiler::MethodDescription.new TestGenerator
  yield desc.generator
  return desc
end
