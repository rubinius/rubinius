shared :lookuptable_clone do |cmd|
  describe "LookupTable##{cmd}" do
    before :each do
      @lt = LookupTable.new(:a => 1, :b => 2, :c => 3)
    end

    it "creates a new instance of LookupTable" do
      c = @lt.send(cmd)
      c.keys.should == @lt.keys
      c.values.should == @lt.values
      c.size.should == @lt.size
    end

    it "creates a new instance with separate internal state" do
      c = @lt.send(cmd)
      c[:d] = 5
      c.size.should == 4
      c.keys.should_not == @lt.keys
      c.values.should_not == @lt.values
    end

    it "creates a valid clone of an empty LookupTable" do
      lt = LookupTable.new
      c = lt.send(cmd)
      c.size.should == lt.size
    end
  end
end
