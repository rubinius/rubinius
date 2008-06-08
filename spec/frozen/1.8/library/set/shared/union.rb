shared :set_union do |cmd|
  describe "Set##{cmd}" do
    before :each do
      @set = Set[:a, :b, :c]
    end
    
    it "returns a new Set containing all elements of self and the passed Enumerable" do
      @set.send(cmd, Set[:b, :d, :e]).should == Set[:a, :b, :c, :d, :e]
      @set.send(cmd, [:b, :e]).should == Set[:a, :b, :c, :e]
    end
    
    it "raises an ArgumentError when passed a non-Enumerable" do
      lambda { @set.send(cmd, 1) }.should raise_error(ArgumentError)
      lambda { @set.send(cmd, Object.new) }.should raise_error(ArgumentError)
    end
  end
end
