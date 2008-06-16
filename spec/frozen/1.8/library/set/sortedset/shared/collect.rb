shared :sorted_set_collect_bang do |cmd|
  describe "SortedSet##{cmd}" do
    before(:each) do
      @set = SortedSet[1, 2, 3, 4, 5]
    end

    it "yields each Object in self in sorted order" do
      res = []
      SortedSet["one", "two", "three"].send(cmd) { |x| res << x; x }
      res.should == ["one", "two", "three"].sort
    end

    it "returns self" do
      @set.send(cmd) { |x| x }.should equal(@set)
    end

    it "replaces self with the return values of the block" do
      @set.send(cmd) { |x| x * 2 }
      @set.should == SortedSet[2, 4, 6, 8, 10]
    end
  end
end