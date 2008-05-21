shared :strscan_get_byte do |cmd|
  describe "StringScanner##{cmd}" do
    it "scans one byte and returns it" do
      s = StringScanner.new('abc5.')
      s.send(cmd).should == 'a'
      s.send(cmd).should == 'b'
      s.send(cmd).should == 'c'
      s.send(cmd).should == '5'
      s.send(cmd).should == '.'
    end

    it "is not multi-byte character sensitive" do
      s = StringScanner.new("\244\242")
      s.send(cmd).should == "\244"
      s.send(cmd).should == "\242"
    end

    it "returns nil at the end of the string" do
      # empty string case
      s = StringScanner.new('')
      s.send(cmd).should == nil
      s.send(cmd).should == nil

      # non-empty string case
      s = StringScanner.new('a')
      s.send(cmd) # skip one
      s.send(cmd).should == nil
    end
  end
end
