shared :sorted_set_add do |cmd|
  describe "SortedSet##{cmd}" do
    before :each do
      @set = SortedSet.new
    end
    
    it "adds the passed Object to self" do
      @set.send(cmd, "dog")
      @set.should include("dog")
    end
    
    it "returns self" do
      @set.send(cmd, "dog").should equal(@set)
    end
  end
end