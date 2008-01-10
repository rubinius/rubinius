shared :stringio_tell do |cmd|
  describe "StringIO##{cmd}" do
    before(:each) do
      @io = StringIOSpecs.build
    end

    it "returns the current byte offset" do
      @io.getc
      @io.send(cmd).should == 1
      @io.read(7)
      @io.send(cmd).should == 8
    end
  end
end
