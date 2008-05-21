shared :module_class_eval do |cmd|
  describe "Module##{cmd}" do
    it "evaluates a given string in the context of self" do
      ModuleSpecs.send(cmd, "self").should == ModuleSpecs
      ModuleSpecs.send(cmd, "1 + 1").should == 2
    end

    it "defines constants in the receiver's scope" do
      ModuleSpecs.send(cmd, "module NewEvaluatedModule;end")
      ModuleSpecs.const_defined?(:NewEvaluatedModule).should == true
    end
    
    it "evaluates a given block in the context of self" do
      ModuleSpecs.send(cmd) { self }.should == ModuleSpecs
      ModuleSpecs.send(cmd) { 1 + 1 }.should == 2
    end
    
    it "uses the optional filename and lineno parameters for error messages" do
      ModuleSpecs.send(cmd, "[__FILE__, __LINE__]", "test", 102).should == ["test", 102]
    end

    it "converts non string eval-string to string using to_str" do
      (o = mock('1 + 1')).should_receive(:to_str).and_return("1 + 1")
      ModuleSpecs.send(cmd, o).should == 2
    end

    it "raises a TypeError when the given eval-string can't be converted to string using to_str" do
      o = mock('x')
      lambda { ModuleSpecs.send(cmd, o) }.should raise_error(TypeError)
    
      (o = mock('123')).should_receive(:to_str).and_return(123)
      lambda { ModuleSpecs.send(cmd, o) }.should raise_error(TypeError)
    end
  
    it "raises an ArgumentError when no arguments and no block are given" do
      lambda { ModuleSpecs.send(cmd) }.should raise_error(ArgumentError)
    end
  
    it "raises an ArgumentError when more than 3 arguments are given" do
      lambda {
        ModuleSpecs.send(cmd, "1 + 1", "some file", 0, "bogus")
      }.should raise_error(ArgumentError)
    end
  
    it "raises an ArgumentError when a block and normal arguments are given" do
      lambda {
        ModuleSpecs.send(cmd, "1 + 1") { 1 + 1 }
      }.should raise_error(ArgumentError)
    end
  end
end
