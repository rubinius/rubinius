shared :lookuptable_has_key do |cmd|
  describe "LookupTable##{cmd}" do
    before :each do
      @lt = LookupTable.new(:a => 1, :b => 2, :c => 3)
    end

    it "returns true if the LookupTable contains the key" do
      @lt.send(cmd, :a).should == true
      @lt.send(cmd, :b).should == true
    end

    it "returns false if the LookupTable does not contain the key" do
      @lt.send(cmd, :d).should == false
    end

    it "converts a String key argument to a Symbol" do
      @lt.send(cmd, "c").should == true
    end
  end
end
