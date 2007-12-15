shared :kernel_object_id do |cmd|
  describe "Object##{cmd}" do
    # #object_id and #__id__ are aliases, so we only need one function
    # that tests both methods
    it "returns an integer" do
      Object.new.send(cmd).class.should == Fixnum      
      nil.send(cmd).class.should == Fixnum      
    end
  
    it "returns the same value on all calls to id for a given object" do
      o1 = Object.new
      o1.send(cmd).should == o1.send(cmd)
    end
  
    it "returns different values for different objects" do
      o1 = Object.new
      o2 = Object.new
      o1.send(cmd).should_not == o2.send(cmd)
    end
  end
end
