shared :enumerable_entries do |cmd|
  describe "Enumerable##{cmd}" do
    before :each do
      @a = EachDefiner.new( 2, 4, 6, 8, 10 )
    end
    
    it "returns an array containing the items in enum." do
      EachDefiner.new().send(cmd).should ==[]
      EachDefiner.new(3).send(cmd).should ==[3]
      @a.send(cmd).should ==@a.arr
      EachDefiner.new(3,4,5).send(cmd).should ==[3,4,5]
      EachDefiner.new(false,3,nil,4,5,nil).send(cmd).should ==[false,3,nil,4,5,nil]
    end
 
    it "entries should return an array containing the elements" do
      Numerous.new(1, nil, 'a', 2, false, true).send(cmd).should == [1, nil, "a", 2, false, true]
    end
  
    it "to_a should should be a synonym for entries" do
      Numerous.new(1, nil, 'a', 2, false, true).to_a.should == Numerous.new(1, nil, 'a', 2, false, true).entries
    end
  end
end
