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

class StringSubclass < String;end

describe "StringIO.open" do
  it "contains an empty string if no argument is provided" do
    StringIO.open.string.should == ""
  end

  it "yields the IO object to the block" do
    sio = nil
    StringIO.open("abc") do |io|
      io.string.should == 'abc'
      io.read(2).should == 'ab'
      io.closed?.should == false
      sio = io
    end
    sio.closed?.should == true
  end

  it "calls to_str on the first argument if it is not a String" do
    obj = mock('hello')
    def obj.to_str; "hello"; end
    StringIO.open(obj) do |io|
      io.string.should == "hello"
    end
  end

  it "raises a TypeError if the argument cannot be converted" do
    obj = mock('x')
    lambda { StringIO.open(obj) }.should raise_error(TypeError)
  end
end

describe "StringIO.new" do
  it "contains an empty string if no argument is provided" do
    StringIO.new.string.should == ""
  end

  it "calls to_str on the first argument if it is not a String" do
    obj = mock('hello')
    def obj.to_str; "hello"; end
    io = StringIO.new(obj)
    io.string.should == "hello"
  end

  it "raises a TypeError if the argument cannot be converted" do
    obj = mock('x')
    lambda { StringIO.new(obj) }.should raise_error(TypeError)
  end

  it "initializes in read-only mode when given a 'read' mode flag" do
    io = StringIO.new('hi', 'r')
    io.closed_write?.should == true
    io.closed_read?.should == false
    io = StringIO.new('bye', 'rb')
    io.closed_write?.should == true
    io.closed_read?.should == false
    lambda { io.write('!') }.should raise_error(IOError)
  end

  it "does not call to_str on String subclasses" do
    str = StringSubclass.new('keep')
    io = StringIO.new(str)
    io.string.class.should == StringSubclass
    io.write('!')
    io.string.class.should == StringSubclass
  end
end

describe "StringIO#<<" do
  it "appends to the string when in append mode" do
    @io = StringIO.new("example", 'a')
    (@io << "x").should == @io
    @io.string.should == "examplex"
  end

  it "writes to the string when in default mode" do
    @io = StringIO.new("example")
    (@io << "x").should == @io
    @io.string.should == "xxample"
  end
end

describe "StringIO#binmode" do
  it "returns the IO object" do
    @io = StringIO.new("example")
    @io.binmode.should == @io
  end
end

describe "StringIO#close" do
  before(:each) do
    util_build_stringio
  end
  it "prevents further operations" do
    @io.close.should == nil
    @io.closed?.should == true
    @io.closed_read?.should == true
    @io.closed_write?.should == true
    lambda { @io << 'x' }.should raise_error(IOError)
  end
end

describe "StringIO#close_read" do
  before(:each) do
    @io = StringIO.new("example")
    @io.close_read
  end

  it "prevents further reading" do
    @io.closed_read?.should == true
    @io.closed_write?.should == false
    lambda { @io.read(1) }.should raise_error(IOError)
  end

  it "allows further writing" do
    @io.write("x").should == 1
  end
end

describe "StringIO#close_write" do
  before(:each) do
    @io = StringIO.new("example")
    @io.close_write
  end

  it "prevents further writing" do
    @io.closed_read?.should == false
    @io.closed_write?.should == true
    lambda { @io.write('x') }.should raise_error(IOError)
  end

  it "allows further reading" do
    @io.read(1).should == 'e'
  end
end

describe "StringIO#each and each_line" do
  before(:each) do
    @io = StringIO.new("a b c d e") 
  end
  
  it "yields each line by default" do
    seen = []
    @io.each {|s| seen << s}.should == @io
    seen.should == ["a b c d e"]
  end

  it "supports a separator argument" do
    seen = []
    @io.each(' ') {|s| seen << s}.should == @io
    seen.should == ["a ", "b ", "c ", "d ", "e"]
  end
end

describe "StringIO#each_byte" do
  before(:each) do
    @io = StringIO.new("xyz")
  end

  it "yields each character code in turn" do
    seen = []
    @io.each_byte {|b| seen << b}.should == nil
    seen.should == [120, 121, 122]
  end

  it "raises an IOError unless the IO is open for reading" do
    @io.close_read
    lambda { @io.each_byte {|b| b } }.should raise_error(IOError)
  end
end

describe "StringIO#eof? and eof" do
  before(:each) do
    @io = StringIO.new("eof")
  end

  it "is false for a new StringIO" do
    @io.eof.should == false
    @io.eof?.should == false
  end

  it "is true once the IO has been read" do
    @io.read(3)
    @io.eof.should == true
    @io.eof?.should == true
  end

  it "is true after gets returns nil" do
    @io.gets.should == 'eof'
    @io.gets.should == nil
    @io.eof.should == true
    @io.eof?.should == true
  end

  it "is true after seeking to the end" do
    @io.seek(3)
    @io.eof.should == true
  end

  it "is true after seeking beyond the end" do
    @io.seek(5)
    @io.eof.should == true
  end

  it "is true after setting the position to the end" do
    @io.pos = 3
    @io.eof.should == true
  end

  it "is true after setting the position beyond the end" do
    @io.pos = 5
    @io.eof.should == true
  end
end

describe "StringIO#fcntl" do
  it "raises a NotImplementedError" do
    @io = StringIO.new("boom")
    lambda { @io.fcntl }.should raise_error(NotImplementedError)
  end
end

describe "StringIO#fileno" do
  it "is nil" do
    @io = StringIO.new("nuffin")
    @io.fileno.should == nil
  end
end

describe "StringIO#flush" do
  it "returns itself" do
    @io = StringIO.new("flush")
    @io.flush.should == @io
  end
end

describe "StringIO#fsync" do
  it "returns zero" do
    @io = StringIO.new("fsync")
    @io.fsync.should == 0
  end
end

describe "StringIO#getc" do
  before(:each) do
    @io = StringIO.new("getc")
  end

  it "returns the next character code" do
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

  after(:each) do
    $/ = "\n"
  end

  it "returns the next 'line'" do
    @io.gets.should == 'g e t s'
    @io.gets.should == nil
  end

  it "raises an IOError when it is not open for reading" do
    @io.close_read
    lambda { @io.gets }.should raise_error(IOError)
  end

  it "supports separator strings" do
    @io.gets('e').should == 'g e'
    @io.gets('e').should == ' t s'
  end

  it "honors the $/ global separator" do
    $/ = ' '
    @io.gets.should == 'g '
    @io.gets.should == 'e '
    @io.gets.should == 't '
    @io.gets.should == 's'
  end
end

describe "StringIO#size and length" do
  before(:each) do
    util_build_stringio
  end

  it "returns the length of the wrapped string" do
    @io.size.should == 37
    @io.length.should == 37
  end
end

describe "StringIO#tty? and isatty" do
  before(:each) do
    @io = StringIO.new('tty')
  end

  it "is false" do
    @io.tty?.should == false
    @io.isatty.should == false
  end
end

describe "StringIO#lineno" do
  before(:each) do
    util_build_stringio
  end

  it "only counts lines that have been read 'line-wise'" do
    @io.read(@io.size)
    @io.lineno.should == 0
  end

  it "matches the newline count after each_line" do
    @io.each_line {|l| l }
    @io.lineno.should == 4
  end

  it "matches the number of 'lines' read, even if they are not lines" do
    @io.gets('a')
    @io.gets('c')
    @io.gets('h')
    6.times { @io.gets }
    @io.lineno.should == 7
  end

  it "updates the line number after readline, as well as gets" do
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

  it "updates the current line number but not advance the position" do
    @io.lineno = 2
    @io.gets.should == "    each\n"
    @io.lineno.should == 3
  end
end

describe "StringIO#path" do
  it "returns nil" do
    StringIO.new("path").path.should == nil
  end
end

describe "StringIO#pid" do
  it "returns nil" do
    StringIO.new("pid").pid.should == nil
  end
end

describe "StringIO#pos and tell" do
  before(:each) do
    util_build_stringio
  end

  it "returns the current byte offset" do
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

  it "updates the current byte offset" do
    @io.pos = 26
    @io.read(1).should == "r"
  end

  it "raises an EINVAL if given a negative argument" do
    lambda { @io.pos = -10 }.should  raise_error(Errno::EINVAL)
  end
end

describe "StringIO#print" do
  before(:each) do
    @io = StringIO.new('')
  end
  after(:each) do
    $\ = nil
  end

  it "prints multiple items to the output" do
    @io.print(5,6,7,8).should == nil
    @io.string.should == '5678'
  end

  it "honors the output record separator global" do
    $\ = 'x'
    @io.print(5,6,7,8).should == nil
    @io.string.should == '5678x'
  end
end

describe "StringIO#printf" do
  before(:each) do
    @io = StringIO.new('')
  end

  it "performs format conversion" do
    @io.printf("%d %04x", 123, 123).should == nil
    @io.string.should == "123 007b"
  end
end

describe "StringIO#putc" do
  before(:each) do
    @io = StringIO.new('')
  end

  it "handles characters and character codes" do
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

  after(:each) do
    $/ = "\n"
  end

  it "writes a newline after objects that do not end in newlines" do
    @io.puts(5).should == nil
    @io.string.should == "5\n"
  end

  it "does not write a newline after objects that end in newlines" do
    @io.puts("5\n").should == nil
    @io.string.should == "5\n"
  end
  
  it "calls to_s before writing non-string objects" do
    object = mock('hola')
    object.should_receive(:to_s).and_return("hola")
    
    @io.puts(object).should == nil
    @io.string.should == "hola\n"
  end
  
  it "writes each arg if given several" do
    @io.puts(1, "two", 3).should == nil
    @io.string.should == "1\ntwo\n3\n"
  end
  
  it "flattens a nested array before writing it" do
    @io.puts([1, 2, [3, [4, [5]]]]).should == nil
    @io.string.should == "1\n2\n3\n4\n5\n"
  end
  
  it "writes [...] for a recursive array arg" do
    x = []
    x << 2 << x
    @io.puts(x).should == nil
    @io.string.should == "2\n[...]\n"
  end
  
  it "writes a newline after objects that do not end in newlines" do
    @io.puts(5).should == nil
    @io.string.should == "5\n"
  end
  
  it "does not write a newline after objects that end in newlines" do
    @io.puts("5\n").should == nil
    @io.string.should == "5\n"
  end

  it "ignores the $/ separator global" do
    $/ = ":"
    @io.puts(5,6)
    @io.string.should == "5\n6\n"
  end
end

describe "StringIO#read" do
  before(:each) do
    util_build_stringio
  end

  it "reads at most 'length' bytes" do
    @io.read(666).should == @io.string
  end

  it "reads to the end of the string if length is omitted" do
    @io.read.should == @io.string
    @io.read.should == ""
    @io.read(nil).should == ""
    @io.read(50).should == nil
  end

  it "only supports String buffers" do
    lambda { @io.read(5, []) }.should raise_error(TypeError)
    @io.pos.should == 0
  end

  it "reads data into a buffer string if provided" do
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

  it "returns character codes one at a time" do
    @io.readchar.should == ?a
    @io.readchar.should == ?b
    @io.readchar.should == ?c
  end

  it "raises an EOFError at the end of the string" do
    3.times { @io.readchar }
    lambda { @io.readchar }.should raise_error(EOFError)
  end
end

describe "StringIO#readline" do
  before(:each) do
    @io = StringIO.new("r e a d")
  end

  it "returns the next 'line'" do
    @io.readline.should == 'r e a d'
  end

  it "raises an EOFError at the end" do
    @io.readline
    lambda { @io.readline }.should raise_error(EOFError)
  end

  it "raises an IOError when it is not open for reading" do
    @io.close_read
    lambda { @io.readline }.should raise_error(IOError)
  end

  it "support separator strings" do
    @io.gets('e').should == 'r e'
    @io.gets('e').should == ' a d'
  end
end

describe "StringIO#readlines" do
  before(:each) do
    @io = StringIO.new("line1\nline2\nline3\n")
  end

  it "returns an array of lines" do
    @io.readlines.should == ["line1\n", "line2\n", "line3\n"]
  end

  it "raises an IOError when it is not open for reading" do
    @io.close_read
    lambda { @io.readlines }.should raise_error(IOError)
  end

  it "returns the rest of the stream when separator is nil" do
    @io.read(4)
    @io.readlines(nil).should == ["1\nline2\nline3\n"]
  end

  it "optionally accepts a separator string" do
    @io.readlines('line').should == ["line", "1\nline", "2\nline", "3\n"]
  end
end

# NOTE: Some reopen specs disabled due to MRI bugs. See:
# http://rubyforge.org/tracker/index.php?func=detail&aid=13919&group_id=426&atid=1698
# for details.
describe "StringIO#reopen" do
  before(:each) do
    @io = StringIO.new('hello','a')
  end

  fails_on :ruby do
    it "reopens a stream when given a String argument" do
      @io.reopen('goodbye').should == @io
      @io.string.should == 'goodbye'
      @io << 'x'
      @io.string.should == 'xoodbye'
    end

    it "reopens a stream in append mode when flagged as such" do
      @io.reopen('goodbye', 'a').should == @io
      @io.string.should == 'goodbye'
      @io << 'x'
      @io.string.should == 'goodbyex'
    end

    it "reopens and truncate when reopened in write mode" do
      @io.reopen('goodbye', 'wb').should == @io
      @io.string.should == ''
      @io << 'x'
      @io.string.should == 'x'
    end

    it "truncates the given string, not a copy" do
      str = 'goodbye'
      @io.reopen(str, 'w')
      @io.string.should == ''
      str.should == ''
    end

    compliant_on :ruby, :jruby do
      it "denies access to prevent truncation of a frozen string" do
        @io = StringIO.new("ice")
        lambda { @io.reopen("burn".freeze, 'w') }.should raise_error(Errno::EACCES)
        lambda { @io.reopen("burn".freeze, 'a') }.should raise_error(Errno::EACCES)
      end

      it "does not raise IOError if a frozen string is passed in read mode" do
        @io.reopen("burn".freeze, 'r')
        @io.string.should == "burn"
      end
    end
  end

  # MRI refuses to convert objects that support to_str, JRuby and Rubinius can
  deviates_on(:jruby, :rubinius) do
    it "calls to_str on the first argument if it is not a String" do
      obj = mock('reopen')
      def obj.to_str; "reopen"; end
      @io.reopen(obj)
      @io.string.should == "reopen"
    end
  end

  it "raises a TypeError if the argument cannot be converted" do
    obj = mock('x')
    lambda { @io.reopen(obj) }.should raise_error(TypeError)
  end
  
  it "reopens a stream when given a new StringIO object" do
    @io.close
    nio = StringIO.new('goodbye')
    @io.reopen(nio)
    @io.closed?.should == false
    @io.string.should == 'goodbye'
  end
end

describe "StringIO#rewind" do
  before(:each) do
    @io = StringIO.new("hello\nworld")
  end

  it "resets the position" do
    @io.gets
    @io.pos.should == 6
    @io.rewind
    @io.pos.should == 0
  end

  it "resets the line number" do
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

  it "seeks to an absolute position" do
    @io.seek(5).should == 0
    @io.read(1).should == '6'
  end

  it "seeks from the current position" do
    @io.read(1)
    @io.seek(1, IO::SEEK_CUR)
    @io.read(1).should == '3'
  end

  it "seeks from the end of the IO" do
    @io.seek(1, IO::SEEK_END)
    @io.read(1).should == nil
    @io.seek(-2, IO::SEEK_END)
    @io.read(1).should == '7'
  end
end

describe "StringIO#string" do
  it "returns the underlying string" do
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

  it "changes the underlying string" do
    str = "hello"
    @io.string = str
    @io.string.should == str
    @io.string.object_id.should == str.object_id
  end

  it "resets the position" do
    @io.read(1)
    @io.pos.should == 1
    @io.string = "other"
    @io.pos.should == 0
    @io.read(1).should == 'o'
  end

  it "resets the line number" do
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

  it "is true" do
    @io.sync.should == true
  end
end

describe "StringIO#sync=" do
  before(:each) do
    @io = StringIO.new('')
  end

  it "does not change 'sync' status" do
    @io.sync = false
    @io.sync.should == true
  end
end

describe "StringIO#sysread" do
  before(:each) do
    util_build_stringio
  end

  it "reads at most 'length' bytes" do
    @io.sysread(666).should == @io.string
  end

  it "reads to the end of the string if length is omitted" do
    @io.sysread.should == @io.string
  end

  it "raises an EOFError after the end of the string" do
    @io.sysread
    lambda { @io.sysread    }.should raise_error(EOFError)
    lambda { @io.sysread(5) }.should raise_error(EOFError)
  end

  it "only supports String buffers" do
    lambda { @io.sysread(5, []) }.should raise_error(TypeError)
    @io.pos.should == 0
  end

  it "reads data into a buffer string if provided" do
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

  it "writes at the current buffer position" do
    @io.read(2)
    @io.write('x').should == 1
    @io.string.should == '12x45'
    @io.syswrite(7).should == 1
    @io.string.should == '12x75'
  end

  it "pads with null bytes if the position is after the end" do
    @io.pos = 8
    @io.write('x')
    @io.string.should == "12345\000\000\000x"
    @io.syswrite(9)
    @io.string.should == "12345\000\000\000x9"
  end

  it "returns the number of bytes written" do
    @io.write('').should == 0
    @io.write(nil).should == 0
    str = "1" * 100
    @io.write(str).should == 100
  end
end

describe "StringIO#truncate" do
  before(:each) do
    @io = StringIO.new('123456789')
  end

  # TODO - ri error - says truncate always returns 0
  it "truncates the underlying string" do
    @io.truncate(4).should == 4
    @io.string.should == '1234'
  end

  it "does not update the position" do
    @io.read(5)
    @io.truncate(3)
    @io.pos.should == 5
  end
end

describe "StringIO#ungetc" do
  before(:each) do
    @io = StringIO.new('1234')
  end

  it "writes a character before the current position" do
    @io.read(1)
    @io.ungetc(65)
    @io.string.should == 'A234'
  end

  it "rewinds the current position by one" do
    @io.read(2)
    @io.pos.should == 2
    @io.ungetc(65)
    @io.pos.should == 1
  end

  it "does nothing when pos == 0" do
    @io.ungetc(65)
    @io.string.should == '1234'
  end

  it "does not accept strings" do
    lambda { @io.ungetc('A') }.should raise_error(TypeError)
  end
end

