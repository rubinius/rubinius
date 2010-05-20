require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

EvalSpecs::A.new.c

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
    EvalSpecs::A::B.name.should == "EvalSpecs::A::B"
  end

  it "evaluates such that consts are scoped to the class of the eval" do
    EvalSpecs::A::C.name.should == "EvalSpecs::A::C"
  end

  it "finds a local in an enclosing scope" do
    a = 1
    eval("a").should == 1
  end

  it "updates a local in an enclosing scope" do
    a = 1
    eval("a = 2").should == 2
    a.should == 2
  end

  it "creates an eval-scope local" do
    eval("eval_only_local = 1; eval_only_local").should == 1
    lambda { eval_only_local }.should raise_error(NameError)
  end

  it "updates a local in a surrounding block scope" do
    EvalSpecs.new.f do
      a = 1
      eval("a = 2").should == 2
      a.should == 2
    end
  end

  it "updates a local in a scope above a surrounding block scope" do
    a = 1
    EvalSpecs.new.f do
      eval("a = 2").should == 2
      a.should == 2
    end
    a.should == 2
  end

  it "updates a local in a scope above when modified in a nested block scope" do
    a = 1
    es = EvalSpecs.new
    eval("es.f { es.f { a = 2 } }").should == 2
    a.should == 2
  end

  ruby_version_is ""..."1.9" do
    it "accepts a Proc object as a binding" do
      x = 1
      bind = proc {}

      eval("x", bind).should == 1
      eval("y = 2", bind).should == 2
      eval("y", bind).should == 2

      eval("z = 3").should == 3
      eval("z", bind).should == 3
    end
  end

  ruby_version_is "1.9" do
    it "doesn't accept a Proc object as a binding" do
      x = 1
      bind = proc {}

      lambda { eval("x", bind) }.should raise_error(TypeError)
    end
  end

  it "does not make Proc locals visible to evaluated code" do
    bind = proc { inner = 4 }
    lambda { eval("inner", bind.binding) }.should raise_error(NameError)
  end

  ruby_version_is ""..."1.9" do
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
  end

  ruby_version_is "1.9" do
    # This differs from the 1.8 example because 1.9 doesn't share scope across
    # sibling evals
    it "allows a binding to be captured inside an eval" do
      outer_binding = binding
      level1 = eval("binding", outer_binding)
      level2 = eval("binding", level1)

      eval("x = 2", outer_binding)
      eval("y = 3", level1)

      eval("w=1", outer_binding)
      eval("w", outer_binding).should == 1
      eval("w=1", level1).should == 1
      eval("w", level1).should == 1
      eval("w=1", level2).should == 1
      eval("w", level2).should == 1

      eval("x", outer_binding).should == 2
      eval("x=2", level1)
      eval("x", level1).should == 2
      eval("x=2", level2)
      eval("x", level2).should == 2

      eval("y=3", outer_binding)
      eval("y", outer_binding).should == 3
      eval("y=3", level1)
      eval("y", level1).should == 3
      eval("y=3", level2)
      eval("y", level2).should == 3
    end
  end

  ruby_version_is ""..."1.9" do
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
  end

  ruby_version_is ""..."1.9" do
    it "allows creating a new class in a binding" do
      bind = proc {}
      eval "class A; end", bind.binding
      eval("A.name", bind.binding).should == "A"
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
  end

  ruby_version_is "1.9" do
    it "allows creating a new class in a binding" do
      bind = proc {}
      eval("class A; end; A.name", bind.binding).should =~ /A$/
    end

    it "allows creating a new class in a binding created by #eval" do
      bind = eval "binding"
      eval("class A; end; A.name", bind).should =~ /A$/
    end

    it "allows creating a new class in a binding returned by a method defined with #eval" do
      bind = eval "def spec_binding; binding; end; spec_binding"
      eval("class A; end; A.name", bind).should =~ /A$/
    end
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
    # The class Object bit is needed to workaround some mspec oddness
    class Object
      [1].each { eval "Const = 1"}
      Const.should == 1
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

  # Found via Rubinius bug github:#149
  it "should not alter the value of __FILE__ in the binding" do
    first_time =  EvalSpecs.call_eval
    second_time = EvalSpecs.call_eval

    # This bug is seen by calling the method twice and comparing the values
    # of __FILE__ each time. If the bug is present, calling eval will set the
    # value of __FILE__ to the eval's "filename" argument.

    second_time.should_not == "(eval)"
    first_time.should == second_time
  end

  deviates_on "jruby" do
    it "can be aliased" do
      alias aliased_eval eval
      x = 2
      aliased_eval('x += 40')
      x.should == 42
    end
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
