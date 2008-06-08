shared :set_include do |cmd|
  describe "Set##{cmd}" do
    it "returns true when self contains the passed Object" do
      set = Set[:a, :b, :c]
      set.send(cmd, :a).should be_true
      set.send(cmd, :e).should be_false
    end
  end
end
