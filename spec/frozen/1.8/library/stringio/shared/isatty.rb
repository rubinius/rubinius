shared :stringio_isatty do |cmd|
  describe "StringIO##{cmd}" do
    before(:each) do
      @io = StringIO.new('tty')
    end

    it "is false" do
      @io.send(cmd).should == false
    end
  end
end
