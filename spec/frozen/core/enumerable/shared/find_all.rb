describe :enumerable_find_all, :shared => true do
  it "returns all elements for which the block is not false" do
    entries = (1..10).to_a
    numerous = EnumerableSpecs::Numerous.new(*entries)
    numerous.send(@method) {|i| i % 3 == 0 }.should == [3, 6, 9]
    numerous.send(@method) {|i| true }.should == (1..10).to_a
    numerous.send(@method) {|i| false }.should == []
  end
end
