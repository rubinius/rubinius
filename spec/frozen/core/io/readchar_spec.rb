require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "IO#readchar" do
  before :each do
    @file_name = File.dirname(__FILE__) + '/fixtures/readlines.txt'
    @file = File.open(@file_name, 'r')
  end

  after :each do
    @file.close unless @file.closed?
  end

  ruby_version_is "" ... "1.9" do
    it "returns the next byte from the stream" do
      @file.readchar.should == 86
      @file.readchar.should == 111
      @file.readchar.should == 105
      # read the rest of line
      @file.readline.should == "ci la ligne une.\n"
      @file.readchar.should == 81
    end
  end

  ruby_version_is "1.9" do
    it "returns the next string from the stream" do
      @file.readchar.should == 'V'
      @file.readchar.should == 'o'
      @file.readchar.should == 'i'
      # read the rest of line
      @file.readline.should == "ci la ligne une.\n"
      @file.readchar.should == 'Q'
    end
  end

  it "raises EOFError when invoked at the end of the stream" do
    # read entire content
    @file.read
    lambda { @file.readchar }.should raise_error(EOFError)
  end

  it "raises EOFError when reaches the end of the stream" do
    lambda { loop { @file.readchar } }.should raise_error(EOFError)
  end

  it "raises EOFError on empty stream" do
    File.open(tmp('empty.txt'), "w+") { |empty|
      lambda {
        empty.readchar
      }.should raise_error(EOFError)
    }

    File.unlink(tmp("empty.txt"))
  end
  
  it "raises IOError on closed stream" do
    lambda { IOSpecs.closed_file.readchar }.should raise_error(IOError)
  end
end
