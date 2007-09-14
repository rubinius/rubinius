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

__END__

describe "StringIO#" do
  before(:each) do
    @io = StringIO.new('')
  end
end
