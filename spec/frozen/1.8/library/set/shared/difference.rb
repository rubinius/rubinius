shared :set_difference do |cmd|
  describe "Set##{cmd}" do
    before :each do
      @set = Set[:a, :b, :c]
    end
    
    it "returns a new Set containting self's elements excluding the elements in the passed Enumerable" do
      @set.send(cmd, Set[:a, :b]).should == Set[:c]
      @set.send(cmd, [:b, :c]).should == Set[:a]
    end
    
    it "raises an ArgumentError when passed a non-Enumerable" do
      lambda { @set.send(cmd, 1) }.should raise_error(ArgumentError)
      lambda { @set.send(cmd, Object.new) }.should raise_error(ArgumentError)
    end
  end
end
