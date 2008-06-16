shared :stringio_length do |cmd|
  describe "StringIO##{cmd}" do
    it "returns the length of the wrapped string" do
      StringIO.new("example").send(cmd).should == 7
    end
  end
end
