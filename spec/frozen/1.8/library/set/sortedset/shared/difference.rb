shared :sorted_set_difference do |cmd|
  describe "SortedSet##{cmd}" do
    before :each do
      @set = SortedSet["a", "b", "c"]
    end
    
    it "returns a new SortedSet containting self's elements excluding the elements in the passed Enumerable" do
      @set.send(cmd, SortedSet["a", "b"]).should == SortedSet["c"]
      @set.send(cmd, ["b", "c"]).should == SortedSet["a"]
    end
    
    it "raises an ArgumentError when passed a non-Enumerable" do
      lambda { @set.send(cmd, 1) }.should raise_error(ArgumentError)
      lambda { @set.send(cmd, Object.new) }.should raise_error(ArgumentError)
    end
  end
end
