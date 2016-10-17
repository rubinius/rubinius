require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

describe "Kernel#instance_eval" do
  before :each do
    ScratchPad.clear
  end

  it "expects a block with no arguments" do
    lambda { "hola".instance_eval }.should raise_error(ArgumentError)
  end

  it "takes no arguments with a block" do
    lambda { "hola".instance_eval(4, 5) {|a,b| a + b } }.should raise_error(ArgumentError)
  end

  it "yields the object to the block" do
    "hola".instance_eval {|o| ScratchPad.record o }
    ScratchPad.recorded.should == "hola"
  end

  it "returns the result of the block" do
    "hola".instance_eval { :result }.should == :result
  end

  it "only binds the eval to the receiver" do
    f = Object.new
    f.instance_eval do
      def foo
        1
      end
    end
    f.foo.should == 1
    lambda { Object.new.foo }.should raise_error(NoMethodError)
  end

  it "preserves self in the original block when passed a block argument" do
    prc = proc { self }

    old_self = prc.call

    new_self = Object.new
    new_self.instance_eval(&prc).should == new_self

    prc.call.should == old_self
  end

  # TODO: This should probably be replaced with a "should behave like" that uses
  # the many scoping/binding specs from kernel/eval_spec, since most of those
  # behaviors are the same for instance_eval. See also module_eval/class_eval.

  it "binds self to the receiver" do
    s = "hola"
    (s == s.instance_eval { self }).should be_true
    o = mock('o')
    (o == o.instance_eval("self")).should be_true
  end

  it "executes in the context of the receiver" do
    "Ruby-fu".instance_eval { size }.should == 7
    "hola".instance_eval("size").should == 4
    Object.class_eval { "hola".instance_eval("to_s") }.should == "hola"
    Object.class_eval { "Ruby-fu".instance_eval{ to_s } }.should == "Ruby-fu"

  end

  it "has access to receiver's instance variables" do
    KernelSpecs::IVars.new.instance_eval { @secret }.should == 99
    KernelSpecs::IVars.new.instance_eval("@secret").should == 99
  end

  it "treats block-local variables as local to the block" do
    prc = instance_eval <<-CODE
      proc do |x, prc|
        if x
          n = 2
        else
          n = 1
          prc.call(true, prc)
          n
        end
      end
    CODE

    prc.call(false, prc).should == 1
  end

  # On 1.9 class variables aren't inherited so we have to modify the test
  # from 1.8
  it "sets class variables in the receiver" do
    KernelSpecs::InstEvalCVar.class_variables.should include(:@@count)
    KernelSpecs::InstEvalCVar.send(:class_variable_get, :@@count).should == 2
  end

  it "makes the receiver metaclass the scoped class when used with a string" do
    obj = Object.new
    klass = obj.instance_eval %{
      class B; end
      B
    }
    obj.singleton_class.const_get(:B).should be_an_instance_of(Class)
  end

  it 'allows evaling local variables when using a block' do
    obj = Object.new
    result = obj.instance_eval do
      foo = 123
      eval('foo')
    end
    result.should == 123
  end

  it 'allows evaling local variables when using a string' do
    obj = Object.new
    result = obj.instance_eval %{
      foo = 123
      eval('foo')
    }
    result.should == 123
  end

  it "gets constants in the receiver if a string given" do
    KernelSpecs::InstEvalOuter::Inner::X_BY_STR.should == 2
  end

  it "doesn't get constants in the receiver if a block given" do
    KernelSpecs::InstEvalOuter::Inner::X_BY_BLOCK.should be_nil
  end

  it "raises a TypeError when defining methods on an immediate" do
    lambda do
      1.instance_eval { def foo; end }
    end.should raise_error(TypeError)
    lambda do
      :foo.instance_eval { def foo; end }
    end.should raise_error(TypeError)
  end

quarantine! do # Not clean, leaves cvars lying around to break other specs
  it "scopes class var accesses in the caller when called on a Fixnum" do
    # Fixnum can take instance vars
    Fixnum.class_eval "@@__tmp_instance_eval_spec = 1"
    (defined? @@__tmp_instance_eval_spec).should be_nil

    @@__tmp_instance_eval_spec = 2
    1.instance_eval { @@__tmp_instance_eval_spec }.should == 2
    Fixnum.__send__(:remove_class_variable, :@@__tmp_instance_eval_spec)
  end
end

  it "raises a TypeError when defining methods on numerics" do
    lambda do
      (1.0).instance_eval { def foo; end }
    end.should raise_error(TypeError)
    lambda do
      (1 << 64).instance_eval { def foo; end }
    end.should raise_error(TypeError)
  end

  it "evaluates procs originating from methods" do
    def meth(arg); arg; end

    m = method(:meth)
    obj = Object.new

    obj.instance_eval(&m).should == obj
  end
end
