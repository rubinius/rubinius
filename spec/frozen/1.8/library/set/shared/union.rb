shared :set_union do |cmd|
  describe "Set##{cmd}" do
    before :each do
      @set = Set[:a, :b, :c]
    end
    
    it "returns a new set containing all the elements of self and other" do
      @set.send(cmd, Set[:b, :d, :e]).should == Set[:a, :b, :c, :d, :e]
    end
    
    it "accepts any enumerable as other" do
      @set.send(cmd, [:b, :e]).should == Set[:a, :b, :c, :e]
    end
  end
end
