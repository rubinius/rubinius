shared :enumerable_find_all do |cmd|
  describe "Enumerable##{cmd}" do 
    it "returns all elements for which the block is not false" do
      entries = (1..10).to_a
      numerous = EnumerableSpecs::Numerous.new(*entries)
      numerous.send(cmd) {|i| i % 3 == 0 }.should == [3, 6, 9]
      numerous.send(cmd) {|i| true }.should == (1..10).to_a
      numerous.send(cmd) {|i| false }.should == []
    end
  end
end
