require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

describe "Kernel#open" do

  before :each do
    @name = tmp("kernel_open.txt")
    touch(@name) { |f| f.write "This is a test" }
    @file = nil
  end

  after :each do
    @file.close if @file
    rm_r @name
  end

  it "is a private method" do
    Kernel.should have_private_instance_method(:open)
  end

  it "opens a file when given a valid filename" do
    @file = open(@name)
    @file.should be_kind_of(File)
  end

  it "opens a file when called with a block" do
    @output = open(@name, "r") { |f| f.gets }
    @output.should == "This is a test"
  end

  platform_is_not :windows do
    it "opens an io when path starts with a pipe" do
      @io = open("|date")
      @io.should be_kind_of(IO)
    end

    it "opens an io when called with a block" do
      @output = open("|date") { |f| f.gets }
      @output.should_not == ''
    end
  end

  platform_is :windows do
    it "opens an io when path starts with a pipe" do
      @io = open("|date /t")
      @io.should be_kind_of(IO)
    end

    it "opens an io when called with a block" do
      @output = open("|date /t") { |f| f.gets }
      @output.should_not == ''
    end
  end

  it "raises an ArgumentError if not passed one argument" do
    lambda { open }.should raise_error(ArgumentError)
  end

  ruby_version_is "1.9" do
    it "calls #to_open on argument" do
      obj = mock('fileish')
      @file = File.open(@name)
      obj.should_receive(:to_open).and_return(@file)
      @file = open(obj)
      @file.should be_kind_of(File)
    end

    it "raises a TypeError if passed a non-String that does not respond to #to_open" do
      obj = mock('non-fileish')
      lambda { open(obj) }.should raise_error(TypeError)
      lambda { open(nil) }.should raise_error(TypeError)
      lambda { open(7)   }.should raise_error(TypeError)
    end
  end

  ruby_version_is ""..."1.9" do
    it "raises a TypeError if not passed a String type" do
      lambda { open(nil)       }.should raise_error(TypeError)
      lambda { open(7)         }.should raise_error(TypeError)
      lambda { open(mock('x')) }.should raise_error(TypeError)
    end
  end
end

describe "Kernel.open" do
  it "needs to be reviewed for spec completeness"
end
