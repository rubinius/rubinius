shared :set_length do |cmd|
  describe "Set##{cmd}" do
    it "returns the number of elements in the set" do
      set = Set[:a, :b, :c]
      set.send(cmd).should == 3
    end
  end
end
