require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "IO#getc" do
  before :each do
    @file_name = File.dirname(__FILE__) + '/fixtures/readlines.txt'
    @file = File.open(@file_name, 'r')
  end

  after :each do
    @file.close unless @file.closed?
  end

  it "returns the next byte from the stream" do
    @file.getc.should == 86
    @file.getc.should == 111
    @file.getc.should == 105
    # read the rest of line
    @file.readline.should == "ci la ligne une.\n"
    @file.getc.should == 81
  end

  it "returns nil when invoked at the end of the stream" do
    # read entire content
    @file.read
    @file.getc.should == nil
  end

  it "returns nil on empty stream" do
    File.open(tmp('empty.txt'), "w+") { |empty|
      empty.getc.should == nil
    }
    File.unlink(tmp("empty.txt"))
  end

  it "raises IOError on closed stream" do
    lambda { IOSpecs.closed_file.getc }.should raise_error(IOError)
  end
end
