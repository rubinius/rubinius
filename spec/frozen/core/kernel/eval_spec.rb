require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

class A
  eval "class B; end"
  def c
    eval "class C; end"
  end
end

A.new.c

describe "Kernel#eval" do
  it "is a private method" do
    Kernel.should have_private_instance_method(:eval)
  end

  it "is a module function" do
    Kernel.respond_to?(:eval).should == true
  end

  it "evaluates the code within" do
    eval("2 + 3").should == 5
  end

  it "evaluates within the scope of the eval" do
    A::B.name.should == "A::B"
  end

  it "evaluates such that consts are scoped to the class of the eval" do
    A::C.name.should == "A::C"
  end

  it "accepts a Proc object as a binding" do
    x = 1
    bind = proc {}

    eval("x", bind).should == 1
    eval("y = 2", bind).should == 2
    eval("y", bind).should == 2

    eval("z = 3").should == 3
    eval("z", bind).should == 3
  end

  it "does not make Proc locals visible to evaluated code" do
    bind = proc { inner = 4 }
    lambda { eval("inner", bind) }.should raise_error(NameError)
  end

  it "allows a binding to be captured inside an eval" do
    outer_binding = binding
    level1 = eval("binding", outer_binding)
    level2 = eval("binding", level1)

    eval("w = 1")
    eval("x = 2", outer_binding)
    eval("y = 3", level1)

    eval("w").should == 1
    eval("w", outer_binding).should == 1
    eval("w", level1).should == 1
    eval("w", level2).should == 1

    eval("x").should == 2
    eval("x", outer_binding).should == 2
    eval("x", level1).should == 2
    eval("x", level2).should == 2

    eval("y").should == 3
    eval("y", outer_binding).should == 3
    eval("y", level1).should == 3
    eval("y", level2).should == 3
  end

  it "allows Proc and binding to be nested in horrible ways" do
    outer_binding = binding
    proc_binding = eval("proc {l = 5; binding}.call", outer_binding)
    inner_binding = eval("proc {k = 6; binding}.call", proc_binding)

    eval("w = 1")
    eval("x = 2", outer_binding)
    eval("yy = 3", proc_binding)
    eval("z = 4", inner_binding)

    eval("w").should == 1
    eval("w", outer_binding).should == 1
    eval("w", proc_binding).should == 1
    eval("w", inner_binding).should == 1

    eval("x").should == 2
    eval("x", outer_binding).should == 2
    eval("x", proc_binding).should == 2
    eval("x", inner_binding).should == 2

    lambda { eval("yy") }.should raise_error(NameError)
    lambda { eval("yy", outer_binding) }.should raise_error(NameError)
    eval("yy", proc_binding).should == 3
    eval("yy", inner_binding).should == 3

    lambda { eval("z") }.should raise_error(NameError)
    lambda { eval("z", outer_binding) }.should raise_error(NameError)
    lambda { eval("z", proc_binding)  }.should raise_error(NameError)
    eval("z", inner_binding).should == 4

    lambda { eval("l") }.should raise_error(NameError)
    lambda { eval("l", outer_binding) }.should raise_error(NameError)
    eval("l", proc_binding).should == 5
    eval("l", inner_binding).should == 5

    lambda { eval("k") }.should raise_error(NameError)
    lambda { eval("k", outer_binding) }.should raise_error(NameError)
    lambda { eval("k", proc_binding)  }.should raise_error(NameError)
    eval("k", inner_binding).should == 6
  end

  it "allows creating a new class in a binding" do
    bind = proc {}
    eval "class A; end", bind
    eval("A.name", bind).should == "A"
  end

  it "allows creating a new class in a binding created by #eval" do
    bind = eval "binding"
    eval "class A; end", bind
    eval("A.name").should == "A"
  end

  it "allows creating a new class in a binding returned by a method defined with #eval" do
    bind = eval "def spec_binding; binding; end; spec_binding"
    eval "class A; end", bind
    eval("A.name").should == "A"
  end

  it "includes file and line information in syntax error" do
    expected = 'speccing.rb'
    lambda {
      eval('if true',TOPLEVEL_BINDING,expected)
    }.should raise_error(SyntaxError) { |e|
      e.message.should =~ /^#{expected}:1:.+/
    }
  end

  it "should perform top level evaluations from inside a block" do
    [1].each { eval "Const = 1"}
    Const.should ==1
    class Object
      remove_const :Const
    end
  end

  it "uses the filename of the binding if none is provided" do
    eval("__FILE__").should == "(eval)"
    eval("__FILE__", binding).should == __FILE__
    eval("__FILE__", binding, "success").should == "success"
    eval("eval '__FILE__', binding").should == "(eval)"
    eval("eval '__FILE__', binding", binding).should == __FILE__
    eval("eval '__FILE__', binding", binding, 'success').should == 'success'
  end
end

describe "Kernel.eval" do
#  TODO: This is how MRI 1.9 and JRuby behave. Bug or feature?
#  it "yields to the block of the method when evaling 'yield' inside it" do
#    class TT
#      def self.call_yield_from_eval_no_binding
#        eval('yield')
#      end
#    end
#
#    (TT.call_yield_from_eval_no_binding {"content"}).should == "content"
#  end

  it "does not pass the block to the method being eval'ed" do
    lambda {
      eval('KernelSpecs::EvalTest.call_yield') { "content" }
    }.should raise_error(LocalJumpError)
  end

  it "needs to be reviewed for spec completeness"
end
