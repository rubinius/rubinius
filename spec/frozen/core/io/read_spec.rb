# encoding: utf-8
require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "IO.read" do
  before :each do
    @fname = "test.txt"
    @contents = "1234567890"
    File.open(@fname, "w") { |f| f.write @contents }
  end

  after :each do
    File.delete @fname if File.exists? @fname
  end

  it "reads the contents of a file" do
    IO.read(@fname).should == @contents
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
    File.delete(@fname) if File.exists?(@fname)
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

describe "IO.read on an empty file" do
  before :each do
    @fname = 'empty_test.txt'
    File.open(@fname, 'w') {|f| 1 }
  end

  after :each do
    File.delete @fname  if File.exists? @fname
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
    @fname = "test.txt"
    @contents = "1234567890"
    open @fname, "w" do |io| io.write @contents end

    @io = open @fname, "r+"
  end

  after :each do
    @io.close
    File.delete(@fname) if File.exists?(@fname)
  end

  it "can be read from consecutively" do
    @io.read(1).should == '1'
    @io.read(2).should == '23'
    @io.read(3).should == '456'
    @io.read(4).should == '7890'
  end

  it "can read lots of data" do
    data = "*" * (8096 * 2 + 1024) # HACK IO::BufferSize

    File.open @fname, 'w' do |io| io.write data end

    actual = nil

    File.open @fname, 'r' do |io|
      actual = io.read
    end

    actual.length.should == data.length
    actual.split('').all? { |c| c == "*" }.should == true
  end

  it "can read lots of data with length" do
    read_length = 8096 * 2 + 1024 # HACK IO::BufferSize
    data = "*" * (read_length + 8096) # HACK same

    File.open @fname, 'w' do |io| io.write data end

    actual = nil

    File.open @fname, 'r' do |io|
      actual = io.read read_length
    end

    actual.length.should == read_length
    actual.split('').all? { |c| c == "*" }.should == true
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
    lambda { IOSpecs.closed_file.read }.should raise_error(IOError)
  end

  it "ignores unicode encoding" do
    begin
      old = $KCODE
      $KCODE = "UTF-8"
      File.open(File.dirname(__FILE__) + '/fixtures/readlines.txt', 'r') do |io|
        io.readline.should == "Voici la ligne une.\n"
        io.read(5).should == "Qui " + [195].pack("C")
      end
    ensure
      $KCODE = old
    end
  end
end
