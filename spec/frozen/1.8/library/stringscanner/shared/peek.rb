shared :strscan_peek do |cmd|
  describe "StringScanner##{cmd}" do
    before :each do
      @s = StringScanner.new('This is a test')
    end

    it "returns at most the specified number of characters from the current position" do
      @s.send(cmd, 4).should == "This"
      @s.pos.should == 0
      @s.pos = 5
      @s.send(cmd, 2).should == "is"
      @s.send(cmd, 1000).should == "is a test"
    end

    it "returns an empty string when the passed argument is zero" do
      @s.send(cmd, 0).should == ""
    end

    it "raises a ArgumentError when the passed argument is negative" do
      lambda { @s.send(cmd, -2) }.should raise_error(ArgumentError)
    end

    it "raises a RangeError when the passed argument is a Bignum" do
      lambda { @s.send(cmd, bignum_value) }.should raise_error(RangeError)
    end
  end
end
