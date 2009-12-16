require File.dirname(__FILE__) + '/../../spec_helper'

ruby_version_is "1.9" do
  class IO
    alias getc_orig getc
    def getc
      s = getc_orig
      s ? s.ord : s
    end
  end
end

describe "IO#ungetc" do
  before :each do
    @file_name = File.dirname(__FILE__) + '/fixtures/readlines.txt'
    @file = File.open(@file_name, 'r')
  end

  after :each do
    @file.close unless @file.closed?
  end

  it "pushes back one character onto stream" do
    @file.getc.should == 86
    @file.ungetc(86)
    @file.getc.should == 86

    @file.ungetc(10)
    @file.getc.should == 10

    @file.getc.should == 111
    @file.getc.should == 105
    # read the rest of line
    @file.readline.should == "ci la ligne une.\n"
    @file.getc.should == 81
    @file.ungetc(99)
    @file.getc.should == 99
  end

  it "pushes back one character when invoked at the end of the stream" do
    # read entire content
    @file.read
    @file.ungetc(100)
    @file.getc.should == 100
  end
  
  it "pushes back one character when invoked at the start of the stream" do
    @file.read(0)
    @file.ungetc(100)
    @file.getc.should == 100
  end

  it "pushes back one character when invoked on empty stream" do
    File.open(tmp('empty.txt'), "w+") { |empty|
      empty.getc().should == nil
      empty.ungetc(10)
      empty.getc.should == 10
    }
    File.unlink(tmp("empty.txt"))
  end

  it "affects EOF state" do
    File.open(tmp('empty.txt'), "w+") { |empty|
      empty.eof?.should == true
      empty.getc.should == nil
      empty.ungetc(100)
      empty.eof?.should == false
    }
    File.unlink(tmp("empty.txt"))
  end

  it "adjusts the stream position" do
    @file.pos.should == 0

    # read one char
    c = @file.getc
    @file.pos.should == 1
    @file.ungetc(c)
    @file.pos.should == 0

    # read all
    @file.read
    pos = @file.pos
    @file.ungetc(98)
    @file.pos.should == pos - 1
  end

  # TODO: file MRI bug
  # Another specified behavior that MRI doesn't follow:
  # "Has no effect with unbuffered reads (such as IO#sysread)."
  #
  #it "has no effect with unbuffered reads" do
  #  length = File.size(@file_name)
  #  content = @file.sysread(length)
  #  @file.rewind
  #  @file.ungetc(100)
  #  @file.sysread(length).should == content
  #end

  it "makes subsequent unbuffered operations to raise IOError" do
    @file.getc
    @file.ungetc(100)
    lambda { @file.sysread(1) }.should raise_error(IOError)
  end

  ruby_version_is "" ... "1.9" do
    it "raises IOError when invoked on stream that was not yet read" do
      lambda { @file.ungetc(100) }.should raise_error(IOError)
    end
  end

  ruby_version_is "1.9" do
    it "returns nil when invoked on stream that was not yet read" do
      @file.ungetc(100).should be_nil
    end
  end

  it "raises IOError on closed stream" do
    @file.getc
    @file.close
    lambda { @file.ungetc(100) }.should raise_error(IOError)
  end
end
