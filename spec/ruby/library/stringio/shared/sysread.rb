describe :stringio_sysread_length, :shared => true do
  before(:each) do
    @io = StringIO.new("example")
  end

  it "raises an EOFError when passed length > 0 and no data remains" do
    @io.read.should == "example"
    lambda { @io.sysread(1) }.should raise_error(EOFError)
  end
end
