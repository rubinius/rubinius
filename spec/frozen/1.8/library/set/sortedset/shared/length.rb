shared :sorted_set_length do |cmd|
  describe "SortedSet##{cmd}" do
    it "returns the number of elements in the set" do
      set = SortedSet["a", "b", "c"]
      set.send(cmd).should == 3
    end
  end
end
