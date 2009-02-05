describe :module_class_eval, :shared => true do
  # TODO: This should probably be replaced with a "should behave like" that uses
  # the many scoping/binding specs from kernel/eval_spec, since most of those
  # behaviors are the same for instance_eval. See also module_eval/class_eval.
  it "shares a scope across sibling evals" do
    a, b = Object.new, Object.new

    result = nil
    a.instance_eval "x = 1"
    lambda do
      b.instance_eval "result = x"
    end.should_not raise_error
    result.should == 1
  end

  it "evaluates a given string in the context of self" do
    ModuleSpecs.send(@method, "self").should == ModuleSpecs
    ModuleSpecs.send(@method, "1 + 1").should == 2
  end

  it "does not add defined methods to other classes" do
    FalseClass.class_eval do
      def foo
        'foo'
      end
    end
    lambda {42.foo}.should raise_error(NoMethodError)
  end

  it "defines constants in the receiver's scope" do
    ModuleSpecs.send(@method, "module NewEvaluatedModule;end")
    ModuleSpecs.const_defined?(:NewEvaluatedModule).should == true
  end
  
  it "evaluates a given block in the context of self" do
    ModuleSpecs.send(@method) { self }.should == ModuleSpecs
    ModuleSpecs.send(@method) { 1 + 1 }.should == 2
  end
  
  it "uses the optional filename and lineno parameters for error messages" do
    ModuleSpecs.send(@method, "[__FILE__, __LINE__]", "test", 102).should == ["test", 102]
  end

  it "converts non string eval-string to string using to_str" do
    (o = mock('1 + 1')).should_receive(:to_str).and_return("1 + 1")
    ModuleSpecs.send(@method, o).should == 2
  end

  it "raises a TypeError when the given eval-string can't be converted to string using to_str" do
    o = mock('x')
    lambda { ModuleSpecs.send(@method, o) }.should raise_error(TypeError)
  
    (o = mock('123')).should_receive(:to_str).and_return(123)
    lambda { ModuleSpecs.send(@method, o) }.should raise_error(TypeError)
  end

  it "raises an ArgumentError when no arguments and no block are given" do
    lambda { ModuleSpecs.send(@method) }.should raise_error(ArgumentError)
  end

  it "raises an ArgumentError when more than 3 arguments are given" do
    lambda {
      ModuleSpecs.send(@method, "1 + 1", "some file", 0, "bogus")
    }.should raise_error(ArgumentError)
  end

  it "raises an ArgumentError when a block and normal arguments are given" do
    lambda {
      ModuleSpecs.send(@method, "1 + 1") { 1 + 1 }
    }.should raise_error(ArgumentError)
  end
end
