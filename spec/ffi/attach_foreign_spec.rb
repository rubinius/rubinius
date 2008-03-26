require File.dirname(__FILE__) + '/../spec_helper'


describe "Module#attach_foreign" do
  before :each do
    @mod = Module.new
  end

  it "provides an interface to FFI.create_function" do
    FFI.should_receive(:create_function).and_return true
    @mod.attach_foreign :int, 'blah', []
  end

  it "passes the type arguments and function name to FFI.create_function" do
    mock = FFI.should_receive :create_function
    mock.with(nil, 'foo', [:double, :string], :int).and_return true

    @mod.attach_foreign :int, 'foo', [:double, :string]
  end

  it "raises an error if more than 6 arguments are specified" do
    l = lambda { @mod.attach_foreign(:int, 'foo', ([:int] * 7))}
    l.should raise_error
  end

  it "raises an error if the function cannot be found" do
    FFI.should_receive(:create_function).and_return nil

    l = lambda { @mod.attach_foreign :int, 'blah', [] }
    l.should raise_error
  end

  it "accepts a hash of additional options as the fourth argument" do
    FFI.should_receive(:create_function).and_return true
    @mod.attach_foreign :int, 'foo', [], :hola => 3
  end

  it "passes library path/name given as :from in the option hash" do
    mock = FFI.should_receive :create_function
    mock.with('lib', 'foo', [:double, :string], :int).and_return true

    @mod.attach_foreign :int, 'foo', [:double, :string], :from => 'lib'
  end

  it "defaults to not specifying a library name or path" do
    mock = FFI.should_receive :create_function
    mock.with(nil, 'foo', [:double, :string], :int).and_return true

    @mod.attach_foreign :int, 'foo', [:double, :string]
  end

  it "creates the method as a class method, named same as the function" do
    o = Object.new
    FFI.should_receive(:create_function).and_return o

    @mod.attach_foreign :int, 'af_test1', []

    # Should go in as a class method
    @mod.metaclass.method_table[:af_test1].equal?(o).should == true
  end

  it "uses :as in option hash as the method name when present" do
    o = Object.new
    FFI.should_receive(:create_function).and_return o

    @mod.attach_foreign :int, 'af_test2', [], :as => :af_test3

    # Should go in as a class method
    @mod.metaclass.method_table[:af_test2].should == nil
    @mod.metaclass.method_table[:af_test3].equal?(o).should == true
  end

  it "overwrites an existing method with same name" do
    o = Object.new
    FFI.should_receive(:create_function).and_return o

    def @mod.old; 'Hi, I am old'; end

    @mod.attach_foreign :int, 'old', []

    # Should go in as a class method
    @mod.metaclass.method_table[:old].equal?(o).should == true
  end
end

