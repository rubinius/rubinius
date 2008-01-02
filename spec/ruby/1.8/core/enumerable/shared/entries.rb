shared :enumerable_entries do |cmd|
  describe "Enumerable##{cmd}" do
    before :each do
      @entries = [2, 4, 6, 8, 10]
      @numerous = EnumerableSpecs::Numerous.new(*@entries)
    end
    
    it "returns an array containing the items in enum." do
      @numerous.send(cmd).should == @entries
    end
 
    it "returns an array containing the elements" do
      EnumerableSpecs::Numerous.new(1, nil, 'a', 2, false, true).send(cmd).should == [1, nil, "a", 2, false, true]
    end
  end
end
