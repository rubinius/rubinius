shared :set_intersection do |cmd|
  describe "Set##{cmd}" do
    before :each do
      @set = Set[:a, :b, :c]
    end

    it "returns a new Set containing only elements shared by self and the passed Enumerable" do
      @set.send(cmd, Set[:b, :c, :d, :e]).should == Set[:b, :c]
      @set.send(cmd, [:b, :c, :d]).should == Set[:b, :c]
    end
    
    it "raises an ArgumentError when passed a non-Enumerable" do
      lambda { @set.send(cmd, 1) }.should raise_error(ArgumentError)
      lambda { @set.send(cmd, Object.new) }.should raise_error(ArgumentError)
    end
  end
end
