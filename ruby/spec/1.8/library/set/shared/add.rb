shared :set_add do |cmd|
  describe "Set##{cmd}" do
    before :each do
      @set = Set.new
    end
    
    it "adds the object to the set and returns self" do
      @set.send(cmd, "dog").should == Set["dog"]
    end
  end
end
