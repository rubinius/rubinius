shared :module_class_eval do |cmd|
  describe "Module##{cmd}" do
    it "evaluates a given string in the context of self" do
      ModuleSpecs.send(cmd, "self").should == ModuleSpecs
      ModuleSpecs.send(cmd, "1 + 1").should == 2
    end
    
    it "evaluates a given block in the context of self" do
      ModuleSpecs.send(cmd) { self }.should == ModuleSpecs
      ModuleSpecs.send(cmd) { 1 + 1 }.should == 2
    end
    
    it "uses the optional filename and lineno parameters for error messages" do
      ModuleSpecs.send(cmd, "[__FILE__, __LINE__]", "test", 102).should == ["test", 102]
    end
  end

  it "converts non string eval-string to string using to_str" do
    (o = Object.new).should_receive(:to_str, :returning => "1 + 1")
    ModuleSpecs.send(cmd, o).should == 2
  end

  it "raises a TypeError when the given eval-string can't be converted to string using to_str" do
    o = Object.new
    should_raise(TypeError) do
      ModuleSpecs.send(cmd, o)
    end
    
    (o = Object.new).should_receive(:to_str, :returning => 123)
    should_raise(TypeError) do
      ModuleSpecs.send(cmd, o)
    end
  end
  
  it "raises an ArgumentError when no arguments and no block are given" do
    should_raise(ArgumentError, "block not supplied") do
      ModuleSpecs.send(cmd)
    end
  end
  
  it "raises an ArgumentError when more than 3 arguments are given" do
    should_raise(ArgumentError) do
      ModuleSpecs.send(cmd, "1 + 1", "some file", 0, "bogus")
    end
  end
  
  it "raises an ArgumentError when a block and normal arguments are given" do
    should_raise(ArgumentError) do
      ModuleSpecs.send(cmd, "1 + 1") { 1 + 1 }
    end
  end
end
