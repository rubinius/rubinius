shared :sorted_set_intersection do |cmd|
  describe "SortedSet##{cmd}" do
    before :each do
      @set = SortedSet["a", "b", "c"]
    end

    it "returns a new SortedSet containing only elements shared by self and the passed Enumerable" do
      @set.send(cmd, SortedSet["b", "c", "d", "e"]).should == SortedSet["b", "c"]
      @set.send(cmd, ["b", "c", "d"]).should == SortedSet["b", "c"]
    end
    
    it "raises an ArgumentError when passed a non-Enumerable" do
      lambda { @set.send(cmd, 1) }.should raise_error(ArgumentError)
      lambda { @set.send(cmd, Object.new) }.should raise_error(ArgumentError)
    end
  end
end
