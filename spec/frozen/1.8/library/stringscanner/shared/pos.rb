shared :strscan_pos do |cmd|
  describe "StringScanner##{cmd}" do
    before :each do
      @s = StringScanner.new("This is a test")
    end

    it "returns the position of the scan pointer" do
      @s.send(cmd).should == 0
      @s.scan_until /This is/
      @s.send(cmd).should == 7
      @s.get_byte
      @s.send(cmd).should == 8
      @s.terminate
      @s.send(cmd).should == 14
    end

    it "returns 0 in the reset position" do
      @s.reset
      @s.send(cmd).should == 0
    end

    it "returns the length of the string in the terminate position" do
      @s.terminate
      @s.send(cmd).should == @s.string.length
    end

  end
end

shared :strscan_pos_set do |cmd|
  describe "StringScanner##{cmd}" do
    before :each do
      @s = StringScanner.new("This is a test")
    end

    it "modify the scan pointer" do      
      @s.send(cmd, 5)
      @s.rest.should == "is a test"
    end

    it "raises a RangeError when the passed argument is out of range" do
      lambda { @s.send(cmd, 20) }.should raise_error(RangeError)
    end
  end
end
