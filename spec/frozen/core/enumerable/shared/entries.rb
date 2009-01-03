describe :enumerable_entries, :shared => true do
  before :each do
    @entries = [2, 4, 6, 8, 10]
    @numerous = EnumerableSpecs::Numerous.new(*@entries)
  end
  
  it "returns an array containing the items in enum." do
    @numerous.send(@method).should == @entries
  end

  it "returns an array containing the elements" do
    numerous = EnumerableSpecs::Numerous.new(1, nil, 'a', 2, false, true)
    numerous.send(@method).should == [1, nil, "a", 2, false, true]
  end
end
