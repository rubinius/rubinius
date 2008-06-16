shared :strscan_rest_size do |cmd|
  describe "StringScanner##{cmd}" do
    before :each do
      @s = StringScanner.new('This is a test')
    end

    it "Returns the length of the rest of the string" do
      @s.send(cmd).should == 14
      @s.scan(/This/)
      @s.send(cmd).should == 10
      @s.terminate
      @s.send(cmd).should == 0
    end

    it "is equivalent to rest.size" do
      @s.scan(/This/)
      @s.send(cmd).should == @s.rest.size
    end
  end
end
