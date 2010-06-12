require File.expand_path('../../../spec_helper', __FILE__)

describe "IO buffer" do
  before :each do
    @fname = "test.txt"
    @contents = "1234567890"

    File.open(@fname, "w") {|io| io.write @contents }
    @io = File.open @fname, "r+"
  end

  after :each do
    @io.close unless @io.closed?
    File.delete(@fname) if File.exists?(@fname)
  end

  it "returns the content up to end-of-file with a respect to the read buffer when more bytes are specified" do
    @io.read(4)
    @io.read(4)
    @io.read(4).should == @contents.slice(8, 2)
  end
end
