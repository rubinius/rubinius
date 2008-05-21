shared :module_equal do |cmd|
  describe "Module##{cmd}(other)" do
    it "returns true if self and the given module are the same" do
      ModuleSpecs.send(cmd, ModuleSpecs).should == true
      ModuleSpecs::Child.send(cmd, ModuleSpecs::Child).should == true
      ModuleSpecs::Parent.send(cmd, ModuleSpecs::Parent).should == true
      ModuleSpecs::Basic.send(cmd, ModuleSpecs::Basic).should == true
      ModuleSpecs::Super.send(cmd, ModuleSpecs::Super).should == true
      
      ModuleSpecs::Child.send(cmd, ModuleSpecs).should == false
      ModuleSpecs::Child.send(cmd, ModuleSpecs::Parent).should == false
      ModuleSpecs::Child.send(cmd, ModuleSpecs::Basic).should == false
      ModuleSpecs::Child.send(cmd, ModuleSpecs::Super).should == false
    end
  end
end
