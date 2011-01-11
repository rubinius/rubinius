require File.dirname(__FILE__) + '/../spec_helper'
require 'mspec/helpers/io'
require 'mspec/helpers/fs'
require 'mspec/helpers/fmode'
require 'mspec/helpers/tmp'

describe IOStub do
  before :each do
    @out = IOStub.new
    @sep = $\
  end

  after :each do
    $\ = @sep
  end

  it "provides a write method" do
    @out.write "this"
    @out.should == "this"
  end

  it "concatenates the arguments sent to write" do
    @out.write "flim ", "flam"
    @out.should == "flim flam"
  end

  it "provides a print method that appends the default separator" do
    $\ = " [newline] "
    @out.print "hello"
    @out.print "world"
    @out.should == "hello [newline] world [newline] "
  end

  it "provides a puts method that appends the default separator" do
    @out.puts "hello", 1, 2, 3
    @out.should == "hello\n1\n2\n3\n"
  end

  it "provides a printf method" do
    @out.printf "%-10s, %03d, %2.1f", "test", 42, 4.2
    @out.should == "test      , 042, 4.2"
  end

  it "provides a flush method that does nothing and returns self" do
    @out.flush.should == @out
  end
end

describe Object, "#new_fd" do
  before :each do
    @name = tmp("io_specs")
  end

  after :each do
    @io.close unless @io.closed?
    rm_r @name
  end

  it "returns a Fixnum that can be used to create an IO instance" do
    fd = new_fd @name
    fd.should be_an_instance_of(Fixnum)

    @io = IO.new fd, fmode('w:utf-8')
    @io.sync = true
    @io.print "io data"

    IO.read(@name).should == "io data"
  end
end

describe Object, "#new_io" do
  before :each do
    @name = tmp("io_specs.txt")
  end

  after :each do
    @io.close if @io and !@io.closed?
    rm_r @name
  end

  it "returns an IO instance" do
    @io = new_io @name
    @io.should be_an_instance_of(IO)
  end

  it "opens the IO for reading if passed 'r'" do
    touch(@name) { |f| f.print "io data" }
    @io = new_io @name, "r"
    @io.read.should == "io data"
    lambda { @io.puts "more data" }.should raise_error(IOError)
  end

  it "opens the IO for writing if passed 'w'" do
    @io = new_io @name, "w"
    @io.sync = true

    @io.print "io data"
    IO.read(@name).should == "io data"
  end
end
