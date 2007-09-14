require File.dirname(__FILE__) + '/../spec_helper'
require 'stringio'

def util_build_stringio
  str = <<-EOS
    each
    peach
    pear
    plum
  EOS
  @io = StringIO.new(str)
end

describe "StringIO.open" do
  it "should yield the IO object to the block" do
    sio = nil
    StringIO.open("abc") do |io|
      io.string.should == 'abc'
      io.read(2).should == 'ab'
      io.closed?.should == false
      sio = io
    end
    sio.closed?.should == true
  end
end

describe "StringIO#<<" do
  it "should append to the string when in append mode" do
    @io = StringIO.new("example", 'a')
    (@io << "x").should == @io
    @io.string.should == "examplex"
  end

  it "should write to the string when in default mode" do
    @io = StringIO.new("example")
    (@io << "x").should == @io
    @io.string.should == "xxample"
  end
end

describe "StringIO#binmode" do
  it "should return the IO object" do
    @io = StringIO.new("example")
    @io.binmode.should == @io
  end
end

describe "StringIO#close" do
  before(:each) do
    util_build_stringio
  end
  it "should prevent further operations" do
    @io.close.should == nil
    @io.closed?.should == true
    @io.closed_read?.should == true
    @io.closed_write?.should == true
    should_raise(IOError) { @io << 'x' }
  end
end

describe "StringIO#close_read" do
  before(:each) do
    @io = StringIO.new("example")
    @io.close_read
  end

  it "should prevent further reading" do
    @io.closed_read?.should == true
    @io.closed_write?.should == false
    should_raise(IOError) { @io.read(1) }
  end

  it "should allow further writing" do
    @io.write("x").should == 1
  end
end

describe "StringIO#close_write" do
  before(:each) do
    @io = StringIO.new("example")
    @io.close_write
  end

  it "should prevent further writing" do
    @io.closed_read?.should == false
    @io.closed_write?.should == true
    should_raise(IOError) { @io.write('x') }
  end

  it "should allow further reading" do
    @io.read(1).should == 'e'
  end
end

describe "StringIO#each and each_line" do
  before(:each) do
    @io = StringIO.new("a b c d e") 
  end
  
  it "should yield each line by default" do
    seen = []
    @io.each {|s| seen << s}.should == @io
    seen.should == ["a b c d e"]
  end

  it "should support a separator argument" do
    seen = []
    @io.each(' ') {|s| seen << s}.should == @io
    seen.should == ["a ", "b ", "c ", "d ", "e"]
  end
end

describe "StringIO#each_byte" do
  before(:each) do
    @io = StringIO.new("xyz")
  end

  it "should yield each character code in turn" do
    seen = []
    @io.each_byte {|b| seen << b}.should == nil
    seen.should == [120, 121, 122]
  end

  it "should raise IOError unless the IO is open for reading" do
    @io.close_read
    should_raise(IOError) { @io.each_byte {|b| b } }
  end
end

describe "StringIO#eof? and eof" do
  before(:each) do
    @io = StringIO.new("eof")
  end

  it "should be false for a new StringIO" do
    @io.eof.should == false
    @io.eof?.should == false
  end

  it "should be true once the IO has been read" do
    @io.read(3)
    @io.eof.should == true
    @io.eof?.should == true
  end
end

describe "StringIO#fcntl" do
  it "should raise NotImplementedError" do
    @io = StringIO.new("boom")
    should_raise(NotImplementedError) { @io.fcntl }
  end
end

describe "StringIO#fileno" do
  it "should be nil" do
    @io = StringIO.new("nuffin")
    @io.fileno.should == nil
  end
end

describe "StringIO#flush" do
  it "should return itself" do
    @io = StringIO.new("flush")
    @io.flush.should == @io
  end
end

describe "StringIO#fsync" do
  it "should return zero" do
    @io = StringIO.new("fsync")
    @io.fsync.should == 0
  end
end

describe "StringIO#getc" do
  before(:each) do
    @io = StringIO.new("getc")
  end

  it "should return the next character code" do
    @io.getc.should == ?g
    @io.getc.should == ?e
    @io.getc.should == ?t
    @io.getc.should == ?c
    @io.getc.should == nil
    @io.getc.should == nil
  end
end

describe "StringIO#gets" do
  before(:each) do
    @io = StringIO.new("g e t s")
  end

  it "should return the next 'line'" do
    @io.gets.should == 'g e t s'
    @io.gets.should == nil
  end

  it "should raise IOError when it is not open for reading" do
    @io.close_read
    should_raise(IOError) { @io.gets }
  end

  it "should support separator strings" do
    @io.gets('e').should == 'g e'
    @io.gets('e').should == ' t s'
  end
end

describe "StringIO#size and length" do
  before(:each) do
    util_build_stringio
  end

  it "should return the length of the wrapped string" do
    @io.size.should == 37
    @io.length.should == 37
  end
end

describe "StringIO#tty? and isatty" do
  before(:each) do
    @io = StringIO.new('tty')
  end

  it "should be false" do
    @io.tty?.should == false
    @io.isatty.should == false
  end
end

describe "StringIO#lineno" do
  before(:each) do
    util_build_stringio
  end

  it "should only count lines that have been read 'line-wise'" do
    @io.read(@io.size)
    @io.lineno.should == 0
  end

  it "should match the newline count after each_line" do
    @io.each_line {|l| l }
    @io.lineno.should == 4
  end

  it "should match the number of 'lines' read, even if they are not lines" do
    @io.gets('a')
    @io.gets('c')
    @io.gets('h')
    6.times { @io.gets }
    @io.lineno.should == 7
  end

  it "should update the line number after readline, as well as gets" do
    @io.readline
    @io.readline
    @io.readline
    @io.lineno.should == 3
  end
end

describe "StringIO#lineno=" do
  before(:each) do
    util_build_stringio
  end

  it "should update the current line number but not advance the position" do
    @io.lineno = 2
    @io.gets.should == "    each\n"
    @io.lineno.should == 3
  end
end

describe "StringIO#path" do
  it "should return nil" do
    StringIO.new("path").path.should == nil
  end
end

describe "StringIO#pid" do
  it "should return nil" do
    StringIO.new("pid").pid.should == nil
  end
end

describe "StringIO#pos and tell" do
  before(:each) do
    util_build_stringio
  end

  it "should return the current byte offset" do
    @io.getc
    @io.pos.should == 1
    @io.read(7)
    @io.tell.should == 8
  end
end

describe "StringIO#pos=" do
  before(:each) do
    util_build_stringio
  end

  it "should update the current byte offset" do
    @io.pos = 26
    @io.read(1).should == "r"
  end
end

describe "StringIO#print" do
  before(:each) do
    @io = StringIO.new('')
  end
  after(:each) do
    $\ = nil
  end

  it "should print multiple items to the output" do
    @io.print(5,6,7,8).should == nil
    @io.string.should == '5678'
  end

  it "should honor the output record separator global" do
    $\ = 'x'
    @io.print(5,6,7,8).should == nil
    @io.string.should == '5678x'
  end
end

describe "StringIO#printf" do
  before(:each) do
    @io = StringIO.new('')
  end

  it "should perform format conversion" do
    @io.printf("%d %04x", 123, 123).should == nil
    @io.string.should == "123 007b"
  end
end

describe "StringIO#putc" do
  before(:each) do
    @io = StringIO.new('')
  end

  it "should handle characters and character codes" do
    @io.putc(65).should == 65
    @io.putc('A').should == 'A'
    @io.putc('AA').should == 'AA'
    @io.string.should == 'AAA'
  end
end

describe "StringIO#puts" do
  before(:each) do
    @io = StringIO.new('')
  end

  it "should write a newline after objects that do not end in newlines" do
    @io.puts(5).should == nil
    @io.string.should == "5\n"
  end

  it "should not write a newline after objects that end in newlines" do
    @io.puts("5\n").should == nil
    @io.string.should == "5\n"
  end
end

describe "StringIO#read" do
  before(:each) do
    util_build_stringio
  end

  it "should read at most 'length' bytes" do
    @io.read(666).should == @io.string
  end

  it "should read to the end of the string if length is omitted" do
    @io.read.should == @io.string
    @io.read.should == ""
    @io.read(nil).should == ""
    @io.read(50).should == nil
  end

  it "should only support String buffers" do
    should_raise(TypeError) { @io.read(5, []) }
    @io.pos.should == 0
  end

  it "should read data into a buffer string if provided" do
    @io = StringIO.new('buffered')
    buf = ""
    @io.read(5, buf).object_id.should == buf.object_id
    buf.should == "buffe"
    @io.read(1, buf)
    buf.should == 'r'
  end
end


describe "StringIO#readchar" do
  before(:each) do
    @io = StringIO.new('abc')
  end

  it "should return character codes one at a time" do
    @io.readchar.should == ?a
    @io.readchar.should == ?b
    @io.readchar.should == ?c
  end

  it "should raise EOFError at the end of the string" do
    3.times { @io.readchar }
    should_raise(EOFError) { @io.readchar }
  end
end

describe "StringIO#readline" do
  before(:each) do
    @io = StringIO.new("r e a d")
  end

  it "should return the next 'line'" do
    @io.readline.should == 'r e a d'
  end

  it "should raise EOFError at the end" do
    @io.readline
    should_raise(EOFError) { @io.readline }
  end

  it "should raise IOError when it is not open for reading" do
    @io.close_read
    should_raise(IOError) { @io.readline }
  end

  it "should support separator strings" do
    @io.gets('e').should == 'r e'
    @io.gets('e').should == ' a d'
  end
end

describe "StringIO#readlines" do
  before(:each) do
    @io = StringIO.new("line1\nline2\nline3\n")
  end

  it "should return an array of lines" do
    @io.readlines.should == ["line1\n", "line2\n", "line3\n"]
  end

  it "should raise IOError when it is not open for reading" do
    @io.close_read
    should_raise(IOError) { @io.readlines }
  end

  it "should return the rest of the stream when separator is nil" do
    @io.read(4)
    @io.readlines(nil).should == ["1\nline2\nline3\n"]
  end

  it "should optionally accept a separator string" do
    @io.readlines('line').should == ["line", "1\nline", "2\nline", "3\n"]
  end
end

describe "StringIO#rewind" do
  before(:each) do
    @io = StringIO.new("hello\nworld")
  end

  it "should reset the position" do
    @io.gets
    @io.pos.should == 6
    @io.rewind
    @io.pos.should == 0
  end

  it "should reset the line number" do
    @io.gets
    @io.lineno.should == 1
    @io.rewind
    @io.lineno.should == 0
  end
end

describe "StringIO#seek" do
  before(:each) do
    @io = StringIO.new("12345678")
  end

  it "should seek to an absolute position" do
    @io.seek(5).should == 0
    @io.read(1).should == '6'
  end

  it "should seek from the current position" do
    @io.read(1)
    @io.seek(1, IO::SEEK_CUR)
    @io.read(1).should == '3'
  end

  it "should seek from the end of the IO" do
    @io.seek(1, IO::SEEK_END)
    @io.read(1).should == nil
    @io.seek(-2, IO::SEEK_END)
    @io.read(1).should == '7'
  end
end

describe "StringIO#string" do
  it "should return the underlying string" do
    str = "hello"
    @io = StringIO.new(str)
    @io.string.should == str
    @io.string.object_id.should == str.object_id
  end
end

describe "StringIO#string=" do
  before(:each) do
    @io = StringIO.new("example\nstring")
  end

  it "should change the underlying string" do
    str = "hello"
    @io.string = str
    @io.string.should == str
    @io.string.object_id.should == str.object_id
  end

  it "should reset the position" do
    @io.read(1)
    @io.pos.should == 1
    @io.string = "other"
    @io.pos.should == 0
    @io.read(1).should == 'o'
  end

  it "should reset the line number" do
    @io.gets
    @io.lineno.should == 1
    @io.string = "other"
    @io.lineno.should == 0
    @io.gets.should == "other"
  end
end

describe "StringIO#sync" do
  before(:each) do
    @io = StringIO.new('')
  end

  it "should be true" do
    @io.sync.should == true
  end
end

describe "StringIO#sync=" do
  before(:each) do
    @io = StringIO.new('')
  end

  it "should not change 'sync' status" do
    @io.sync = false
    @io.sync.should == true
  end
end

describe "StringIO#sysread" do
  before(:each) do
    util_build_stringio
  end

  it "should read at most 'length' bytes" do
    @io.sysread(666).should == @io.string
  end

  it "should read to the end of the string if length is omitted" do
    @io.sysread.should == @io.string
  end

  it "should raise EOFError after the end of the string" do
    @io.sysread
    should_raise(EOFError) { @io.sysread }
    should_raise(EOFError) { @io.sysread(5) }
  end

  it "should only support String buffers" do
    should_raise(TypeError) { @io.sysread(5, []) }
    @io.pos.should == 0
  end

  it "should read data into a buffer string if provided" do
    @io = StringIO.new('buffered')
    buf = ""
    @io.sysread(5, buf).object_id.should == buf.object_id
    buf.should == "buffe"
  end
end

describe "StringIO#write and syswrite" do
  before(:each) do
    @io = StringIO.new('12345')
  end

  it "should write at the current buffer position" do
    @io.read(2)
    @io.write('x').should == 1
    @io.string.should == '12x45'
    @io.syswrite(7).should == 1
    @io.string.should == '12x75'
  end

  it "should pad with null bytes if the position is after the end" do
    @io.pos = 8
    @io.write('x')
    @io.string.should == "12345\000\000\000x"
    @io.syswrite(9)
    @io.string.should == "12345\000\000\000x9"
  end
end

describe "StringIO#truncate" do
  before(:each) do
    @io = StringIO.new('123456789')
  end

  # TODO - ri error - says truncate always returns 0
  it "should truncate the underlying string" do
    @io.truncate(4).should == 4
    @io.string.should == '1234'
  end

  it "should not update the position" do
    @io.read(5)
    @io.truncate(3)
    @io.pos.should == 5
  end
end

describe "StringIO#ungetc" do
  before(:each) do
    @io = StringIO.new('1234')
  end

  it "should write a character before the current position" do
    @io.read(1)
    @io.ungetc(65)
    @io.string.should == 'A234'
  end

  it "should rewind the current position by one" do
    @io.read(2)
    @io.pos.should == 2
    @io.ungetc(65)
    @io.pos.should == 1
  end

  it "should do nothing when pos == 0" do
    @io.ungetc(65)
    @io.string.should == '1234'
  end

  it "should not accept strings" do
    should_raise(TypeError) { @io.ungetc('A') }
  end
end

__END__

# TODO - Reopen is utterly broken in MRI right now
# File bug report
describe "StringIO#reopen" do
  before(:each) do
    @io = StringIO.new('hello','a')
  end

  it "should reopen a stream in the same mode" do
    @io.reopen('goodbye').should == @io
    @io.string.should == 'goodbye'
    @io << 'x'
    @io.string.should == 'goodbyex'
  end

  it "should reopen a stream in a different mode" do
    @io.reopen('goodbye', 'w').should == @io
    @io.string.should == 'goodbye'
    @io << 'x'
    @io.string.should == 'xoodbye'
  end
end

