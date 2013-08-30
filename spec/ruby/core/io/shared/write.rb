# encoding: utf-8
require File.expand_path('../../fixtures/classes', __FILE__)

describe :io_write, :shared => true do
  before :each do
    @filename = tmp("IO_syswrite_file") + $$.to_s
    File.open(@filename, "w") do |file|
      file.send(@method, "012345678901234567890123456789")
    end
    @file = File.open(@filename, "r+")
    @readonly_file = File.open(@filename)
  end

  after :each do
    @file.close
    @readonly_file.close
    rm_r @filename
  end

  it "coerces the argument to a string using to_s" do
    (obj = mock('test')).should_receive(:to_s).and_return('a string')
    @file.send(@method, obj)
  end

  it "checks if the file is writable if writing more than zero bytes" do
    lambda { @readonly_file.send(@method, "abcde") }.should raise_error(IOError)
  end

  it "returns the number of bytes written" do
    written = @file.send(@method, "abcde")
    written.should == 5
  end

  it "invokes to_s on non-String argument" do
    data = "abcdefgh9876"
    (obj = mock(data)).should_receive(:to_s).and_return(data)
    @file.send(@method, obj)
    @file.seek(0)
    @file.read(data.size).should == data
  end

  it "writes all of the string's bytes without buffering if mode is sync" do
    @file.sync = true
    written = @file.send(@method, "abcde")
    written.should == 5
    File.open(@filename) do |file|
      file.read(10).should == "abcde56789"
    end
  end

  it "does not warn if called after IO#read" do
    @file.read(5)
    lambda { @file.send(@method, "fghij") }.should_not complain
  end

  it "writes to the current position after IO#read" do
    @file.read(5)
    @file.send(@method, "abcd")
    @file.rewind
    @file.read.should == "01234abcd901234567890123456789"
  end

  it "advances the file position by the count of given bytes" do
    @file.send(@method, "abcde")
    @file.read(10).should == "5678901234"
  end

  it "raises IOError on closed stream" do
    lambda { IOSpecs.closed_io.send(@method, "hello") }.should raise_error(IOError)
  end

  with_feature :encoding do
    before :each do
      @external = Encoding.default_external
      @internal = Encoding.default_internal

      Encoding.default_external = Encoding::UTF_8
    end

    after :each do
      Encoding.default_external = @external
      Encoding.default_internal = @internal
    end

    it "returns the number of bytes written" do
      @file.send(@method, "hellø").should == 6
    end

    it "uses the encoding from the given option for non-ascii encoding" do
      File.open(@filename, "w", :encoding => Encoding::UTF_32LE) do |file|
        file.write("hi").should == 8
      end
      File.binread(@filename).should == "h\u0000\u0000\u0000i\u0000\u0000\u0000"
    end

    it "raises a invalid byte sequence error if invalid bytes are being written" do
      File.open(@filename, "w", :encoding => Encoding::US_ASCII) do |file|
        lambda { file.write("\xFEhi") }.should raise_error(Encoding::InvalidByteSequenceError)
      end
    end

  end
end
