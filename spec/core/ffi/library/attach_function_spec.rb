require File.expand_path('../../../../spec_helper', __FILE__)
require 'rubinius/ffi'

describe "Rubinius::FFI::Library#attach_function" do
  it "needs to be reviewed for spec completeness"

  before :each do
    @libc = Module.new
    @libc.extend Rubinius::FFI::Library
    @libc.ffi_lib Rubinius::FFI::Library::LIBC
  end

  it "creates a module method wrapping the function found in the ffi_lib" do
    @libc.attach_function(:strlen, [:string], :int)
    string = "a string to measure"
    @libc.strlen(string).should == string.size
  end

  it "creates a module method with a different name if given" do
    @libc.attach_function(:measure_a_string, :strlen, [:string], :int)
    string = "a string to measure"
    @libc.measure_a_string(string).should == string.size
  end

  it "accepts an options hash at the end of the three-argument form" do
    options = {:foo => :bar}
    @libc.attach_function(:strlen, [:string], :int, options)
    string = "a string to measure"
    @libc.strlen(string).should == string.size
  end

  it "accepts an options hash at the end of the four-argument form" do
    options = {:foo => :bar}
    @libc.attach_function(:measure_a_string, :strlen, [:string], :int, options)
    string = "a string to measure"
    @libc.measure_a_string(string).should == string.size
  end

  it "raises Rubinius::FFI::NotFoundError if the function could not be found" do
    lambda {
      @libc.attach_function(:unknown_function, [], :void)
    }.should raise_error Rubinius::FFI::NotFoundError
  end

  describe "the attached function module method" do
    before :each do
      @libc.attach_function(:strlen, [:string], :int)
    end

    it "raises TypeError if the wrong argument type is passed" do
      lambda { @libc.strlen(88) }.should raise_error TypeError
    end

    it "raises ArgumentError if too many arguments are passed" do
      lambda { @libc.strlen("foo", "bar") }.should raise_error ArgumentError
    end

    it "raises ArgumentError if too few arguments are passed" do
      lambda { @libc.strlen() }.should raise_error ArgumentError
    end
  end
end
