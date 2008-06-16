shared :strscan_matched_size do |cmd|
  describe "StringScanner##{cmd}" do
    before :each do
      @s = StringScanner.new("This is a test")
    end
    
   it "returns the size of the most recent match" do
      @s.check /This/
      @s.send(cmd).should == 4
      @s.send(cmd).should == 4
      @s.scan //
      @s.send(cmd).should == 0
    end

    it "returns nil if there was no recent match" do
      @s.send(cmd).should == nil
      @s.check /\d+/
      @s.send(cmd).should == nil
      @s.terminate
      @s.send(cmd).should == nil
    end
  end
end
