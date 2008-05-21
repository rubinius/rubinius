shared :set_difference do |cmd|
  describe "Set##{cmd}" do
    before :each do
      @set = Set[:a, :b, :c]
    end
    
    it "returns a new set excluding the elements in other" do
      @set.send(cmd, Set[:a, :b]).should == Set[:c]
    end
    
    it "accepts any enumerable as other" do
      @set.send(cmd, [:b, :c]).should == Set[:a]
    end
  end
end
