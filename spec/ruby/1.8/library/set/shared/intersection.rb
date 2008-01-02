shared :set_intersection do |cmd|
  describe "Set##{cmd}" do
    before :each do
      @set = Set[:a, :b, :c]
    end

    it "returns a new set containing only elements shared by self and other" do
      @set.send(cmd, Set[:b, :c, :d, :e]).should == Set[:b, :c]
    end
    
    it "accepts any enumerable as other" do
      @set.send(cmd, [:b, :c, :d]).should == Set[:b, :c]
    end
  end
end
