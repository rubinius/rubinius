shared :sorted_set_union do |cmd|
  describe "SortedSet##{cmd}" do
    before :each do
      @set = SortedSet["a", "b", "c"]
    end
    
    it "returns a new SortedSet containing all elements of self and the passed Enumerable" do
      @set.send(cmd, SortedSet["b", "d", "e"]).should == SortedSet["a", "b", "c", "d", "e"]
      @set.send(cmd, ["b", "e"]).should == SortedSet["a", "b", "c", "e"]
    end
    
    it "raises an ArgumentError when passed a non-Enumerable" do
      lambda { @set.send(cmd, 1) }.should raise_error(ArgumentError)
      lambda { @set.send(cmd, Object.new) }.should raise_error(ArgumentError)
    end
  end
end
