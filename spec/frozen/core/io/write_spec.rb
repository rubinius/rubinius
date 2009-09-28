require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'
require File.dirname(__FILE__) + '/shared/write'

describe "IO#write on a file" do
  before :each do
    @filename = tmp("IO_syswrite_file") + $$.to_s
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

  # TODO: impl detail? discuss this with matz. This spec is useless. - rdavis
  # I agree. I've marked it not compliant on macruby, as we don't buffer input. -pthomson
  not_compliant_on :macruby do
    it "writes all of the string's bytes but buffers them" do
      written = @file.write("abcde")
      written.should == 5
      File.open(@filename) do |file|
        file.read.should == "012345678901234567890123456789"
        @file.fsync
        file.rewind
        file.read.should == "abcde5678901234567890123456789"
      end
    end
  end

  it "does not check if the file is writable if writing zero bytes" do
    lambda { @readonly_file.write("") }.should_not raise_error
  end

  it "returns a length of 0 when writing a blank string" do
    @file.write('').should == 0
  end
end

describe "IO#write" do
  it_behaves_like :io_write, :write
end
