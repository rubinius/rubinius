require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "IO#write on a file" do
  before :each do
    @filename = "/tmp/IO_syswrite_file" + $$.to_s
    File.open(@filename, "w") do |file|
      file.write("012345678901234567890123456789")
    end
    @file = File.open(@filename, "r+")
    @readonly_file = File.open(@filename)
  end

  after :each do
    @file.close
    @readonly_file.close
    File.delete(@filename)
  end

  it "coerces the argument to a string using to_s" do
    (obj = mock('test')).should_receive(:to_s)
    @file.write(obj)
  end

  it "checks if the file is writable if writing more than zero bytes" do
    lambda { @readonly_file.write("abcde") }.should raise_error(IOError)
  end

  it "does not check if the file is writable if writing zero bytes" do
    lambda { @readonly_file.write("") }.should_not raise_error
  end

  it "returns the number of bytes written" do
    written = @file.write("abcde")
    written.should == 5
  end

  it "writes all of the string's bytes but buffers them" do
    written = @file.write("abcde")
    written.should == 5
    File.open(@filename) do |file|
      file.sysread(10).should_not == "abcde56789"
      file.seek(0)
      @file.fsync
      file.sysread(10).should == "abcde56789"
    end
  end

  it "writes all of the string's bytes without buffering if mode is sync" do
    @file.sync = true
    written = @file.write("abcde")
    written.should == 5
    File.open(@filename) do |file|
      file.read(10).should == "abcde56789"
    end
  end

  it "raises IOError on closed stream" do
    lambda { IOSpecs.closed_file.write("hello") }.should raise_error(IOError)
  end
end
