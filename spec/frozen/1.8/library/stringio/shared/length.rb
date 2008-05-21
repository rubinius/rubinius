shared :stringio_length do |cmd|
  describe "StringIO##{cmd}" do
    before(:each) do
      @io = StringIOSpecs.build
    end

    it "returns the length of the wrapped string" do
      @io.send(cmd).should == 37
    end
  end
end
