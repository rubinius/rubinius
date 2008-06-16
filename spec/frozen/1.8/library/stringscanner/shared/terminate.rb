shared :strscan_terminate do |cmd|
  describe "StringScanner##{cmd}" do
    it "set the scan pointer to the end of the string and clear matching data." do
      s = StringScanner.new('This is a test')
      s.send(cmd)
      s.bol?.should be_false
      s.eos?.should be_true
    end
  end
end
