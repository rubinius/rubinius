# -*- encoding: utf-8 -*-
require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

describe "IO.read" do
  before :each do
    @fname = tmp("io_read.txt")
    @contents = "1234567890"
    touch(@fname) { |f| f.write @contents }
  end

  after :each do
    rm_r @fname
  end

  it "reads the contents of a file" do
    IO.read(@fname).should == @contents
  end

  ruby_version_is "1.9" do
    it "calls #to_path on non-String arguments" do
      p = mock('path')
      p.should_receive(:to_path).and_return(@fname)
      IO.read(p)
    end
  end

  it "treats second nil argument as no length limit" do
    IO.read(@fname, nil).should == @contents
    IO.read(@fname, nil, 5).should == IO.read(@fname, @contents.length, 5)
  end

  it "treats third nil argument as 0" do
    IO.read(@fname, nil, nil).should == @contents
    IO.read(@fname, 5, nil).should == IO.read(@fname, 5, 0)
  end

  it "reads the contents of a file up to a certain size when specified" do
    IO.read(@fname, 5).should == @contents.slice(0..4)
  end

  it "reads the contents of a file from an offset of a specific size when specified" do
    IO.read(@fname, 5, 3).should == @contents.slice(3, 5)
  end

  it "returns nil at end-of-file when length is passed" do
    IO.read(@fname, 1, 10).should == nil
  end

  it "raises an Errno::ENOENT when the requested file does not exist" do
    rm_r @fname
    lambda { IO.read @fname }.should raise_error(Errno::ENOENT)
  end

  it "raises a TypeError when not passed a String type" do
    lambda { IO.read nil }.should raise_error(TypeError)
  end

  it "raises an ArgumentError when not passed a valid length" do
    lambda { IO.read @fname, -1 }.should raise_error(ArgumentError)
  end

  it "raises an Errno::EINVAL when not passed a valid offset" do
    lambda { IO.read @fname, 0, -1  }.should raise_error(Errno::EINVAL)
    lambda { IO.read @fname, -1, -1 }.should raise_error(Errno::EINVAL)
  end
end

describe "IO.read from a pipe" do
  it "runs the rest as a subprocess and returns the standard output" do
    IO.read("|sh -c 'echo hello'").should == "hello\n"
  end

  it "opens a pipe to a fork if the rest is -" do
    str = IO.read("|-")
    if str # parent
      str.should == "hello from child\n"
    else #child
      puts "hello from child"
      exit!
    end
  end

  it "reads only the specified number of bytes requested" do
    IO.read("|sh -c 'echo hello'", 1).should == "h"
  end

  it "raises Errno::ESPIPE if passed an offset" do
    lambda {
      IO.read("|sh -c 'echo hello'", 1, 1)
    }.should raise_error(Errno::ESPIPE)
  end
end

describe "IO.read on an empty file" do
  before :each do
    @fname = tmp("io_read_empty.txt")
    touch(@fname)
  end

  after :each do
    rm_r @fname
  end

  it "returns nil when length is passed" do
    IO.read(@fname, 1).should == nil
  end

  it "returns an empty string when no length is passed" do
    IO.read(@fname).should == ""
  end
end

describe "IO#read" do

  before :each do
    @fname = tmp("io_read.txt")
    @contents = "1234567890"
    touch(@fname) { |f| f.write @contents }

    @io = open @fname, "r+"
  end

  after :each do
    @io.close
    rm_r @fname
  end

  it "can be read from consecutively" do
    @io.read(1).should == '1'
    @io.read(2).should == '23'
    @io.read(3).should == '456'
    @io.read(4).should == '7890'
  end

  it "consumes zero bytes when reading zero bytes" do
    pre_pos = @io.pos

    @io.read(0).should == ''

    @io.getc.chr.should == '1'
  end

  it "is at end-of-file when everything has been read" do
    @io.read
    @io.eof?.should == true
  end

  it "reads the contents of a file" do
    @io.read.should == @contents
  end

  it "places the specified number of bytes in the buffer" do
    buf = ""
    @io.read 5, buf

    buf.should == "12345"
  end

  it "expands the buffer when too small" do
    buf = "ABCDE"
    @io.read nil, buf

    buf.should == @contents
  end

  it "overwrites the buffer" do
    buf = "ABCDEFGHIJ"
    @io.read nil, buf

    buf.should == @contents
  end

  it "truncates the buffer when too big" do
    buf = "ABCDEFGHIJKLMNO"
    @io.read nil, buf
    buf.should == @contents

    @io.rewind

    buf = "ABCDEFGHIJKLMNO"
    @io.read 5, buf
    buf.should == @contents[0..4]
  end

  it "returns the given buffer" do
    buf = ""

    @io.read(nil, buf).object_id.should == buf.object_id
  end

  it "coerces the second argument to string and uses it as a buffer" do
    buf = "ABCDE"
    obj = mock("buff")
    obj.should_receive(:to_str).any_number_of_times.and_return(buf)

    @io.read(15, obj).object_id.should_not == obj.object_id
    buf.should == @contents
  end

  it "returns an empty string at end-of-file" do
    @io.read
    @io.read.should == ''
  end

  it "reads the contents of a file when more bytes are specified" do
    @io.read(@contents.length + 1).should == @contents
  end

  it "returns an empty string at end-of-file" do
    @io.read
    @io.read.should == ''
  end

  it "returns an empty string when the current pos is bigger than the content size" do
    @io.pos = 1000
    @io.read.should == ''
  end

  it "returns nil at end-of-file with a length" do
    @io.read
    @io.read(1).should == nil
  end

  it "with length argument returns nil when the current pos is bigger than the content size" do
    @io.pos = 1000
    @io.read(1).should == nil
  end

  it "raises IOError on closed stream" do
    lambda { IOSpecs.closed_io.read }.should raise_error(IOError)
  end
end

describe "IO#read with encodings" do
  before :each do
    @kcode, $KCODE = $KCODE, "utf-8"
    @io = IOSpecs.io_fixture "lines.txt"
  end

  after :each do
    $KCODE = @kcode
  end

  it "ignores unicode encoding" do
    @io.readline.should == "Voici la ligne une.\n"
    @io.read(5).should == encode("Qui \303", "binary")
  end
end

ruby_version_is "1.9" do
  describe "IO#read with 1.9 encodings" do
    before :each do
      @file = tmp("io_read_bom.txt")
      @text = "\uFEFFT"
    end

    after :each do
      rm_r @file
    end

    # Example derived from test/ruby/test_io_m17n.rb on MRI
    it "strips the BOM when given 'rb:utf-7-bom' as the mode" do
      %w/UTF-8 UTF-16BE UTF-16LE UTF-32BE UTF-32LE/.each do |encoding|
        content = @text.encode(encoding)
        content_ascii = content[1].force_encoding("ascii-8bit")
        touch(@file) { |f| f.print content }

        result = File.read(@file, :mode => "rb:BOM|#{encoding}")
        result.force_encoding("ascii-8bit").should == content_ascii
      end
    end
  end
end

describe "IO#read with large data" do
  before :each do
    # TODO: what is the significance of this mystery math?
    @data_size = 8096 * 2 + 1024
    @data = "*" * @data_size

    @fname = tmp("io_read.txt")
    touch(@fname) { |f| f.write @data }

    @io = open @fname, "r"
  end

  after :each do
    @io.close
    rm_r @fname
  end

  it "reads all the data at once" do
    File.open(@fname, 'r') { |io| ScratchPad.record io.read }

    ScratchPad.recorded.size.should == @data_size
    ScratchPad.recorded.should == @data
  end

  it "reads only the requested number of bytes" do
    read_size = @data_size / 2
    File.open(@fname, 'r') { |io| ScratchPad.record io.read(read_size) }

    ScratchPad.recorded.size.should == read_size
    ScratchPad.recorded.should == @data[0, read_size]
  end
end
