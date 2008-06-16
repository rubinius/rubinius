shared :strscan_eos do |cmd|
  describe "StringScanner##{cmd}" do
    before :each do
      @s = StringScanner.new("This is a test")
    end

    it "Returns true if the scan pointer is at the end of the string" do
      @s.terminate
      @s.send(cmd).should be_true

      s = StringScanner.new('')
      s.send(cmd).should be_true
    end

    it "Returns false if the scan pointer is not at the end of the string" do
      @s.send(cmd).should be_false
    end
  end
end
