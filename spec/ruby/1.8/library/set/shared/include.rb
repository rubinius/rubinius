shared :set_include do |cmd|
  describe "Set##{cmd}" do
    it "returns true if the set contains the object" do
      set = Set[:a, :b, :c]
      set.send(cmd, :a).should == true
      set.send(cmd, :e).should == false
    end
  end
end
