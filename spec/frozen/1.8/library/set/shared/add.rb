shared :set_add do |cmd|
  describe "Set##{cmd}" do
    before :each do
      @set = Set.new
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