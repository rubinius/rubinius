require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "IO#eof?" do
  before :each do
    @file_name = File.dirname(__FILE__) + '/fixtures/readlines.txt'
    @file = File.open(@file_name, 'r')
  end

  after :each do
    @file.close unless @file.closed?
  end

  it "returns false when not at end of file" do
    @file.read 1
    @file.eof?.should == false
  end

  it "returns true after reading with read with no parameters" do
    @file.read()
    @file.eof?.should == true
  end

  it "returns true after reading with read" do
    @file.read(File.size(@file_name))
    @file.eof?.should == true
  end

  it "returns true after reading with sysread" do
    @file.sysread(File.size(@file_name))
    @file.eof?.should == true
  end

  it "returns true after reading with readlines" do
    @file.readlines
    @file.eof?.should == true
  end

  it "returns true on just opened empty stream" do
    File.open(tmp('empty.txt'), "w") { |empty| } # ensure it exists
    File.open(tmp('empty.txt')) { |empty|
      empty.eof?.should == true
    }
    File.unlink(tmp("empty.txt"))
  end

  it "returns false on just opened non-empty stream" do
    @file.eof?.should == false
  end

  ruby_version_is ""..."1.9" do
    it "should not consume the data from the stream" do
      @file.eof?.should == false
      @file.getc.should == 86
    end
  end

  ruby_version_is "1.9" do
    it "should not consume the data from the stream" do
      @file.eof?.should == false
      @file.getc.should == 'V'
    end
  end

  it "raises IOError on closed stream" do
    lambda { IOSpecs.closed_file.eof? }.should raise_error(IOError)
  end

  it "raises IOError on stream not opened for reading" do
 
    File.open(tmp('empty.txt'), "w") { |empty|
      lambda {
        empty.eof?
      }.should raise_error(IOError)
    }
    File.unlink(tmp("empty.txt"))
  end

  it "raises IOError on stream closed for reading by close_read" do
    @file.close_read
    lambda { @file.eof? }.should raise_error(IOError)
  end

  it "returns true on one-byte stream after single-byte read" do
    File.open(File.dirname(__FILE__) + '/fixtures/one_byte.txt') { |one_byte|
      one_byte.read(1)
      one_byte.eof?.should == true
    }
  end

  it "returns true on receiving side of Pipe when writing side is closed" do
    r, w = IO.pipe
    w.close
    r.eof?.should == true
    r.close
  end

  it "returns false on receiving side of Pipe when writing side wrote some data" do
    r, w = IO.pipe
    w.puts "hello"
    r.eof?.should == false
    w.close
    r.eof?.should == false
    r.read
    r.eof?.should == true
    r.close
  end
end
