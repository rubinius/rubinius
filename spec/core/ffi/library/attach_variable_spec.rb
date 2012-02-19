require File.expand_path('../../spec_helper', __FILE__)

describe FFI::Library, "#attach_variable" do
  extend FFISpecs::LibraryHelper
  
  it "should raise an exception if a variable does not exist" do
    lambda {
      extended_module do
        attach_variable :nonexistent, "nonexistent", :char
      end
    }.should raise_error(FFI::NotFoundError)
  end

  [ 0, 127, -128, -1 ].each do |i|
    it "should correctly operate with :char variable" do
      gvar_test("s8", :char, i)
    end
  end

  [ 0, 0x7f, 0x80, 0xff ].each do |i|
    it "should correctly operate with :uchar variable" do
      gvar_test("u8", :uchar, i)
    end
  end

  [ 0, 0x7fff, -0x8000, -1 ].each do |i|
    it "should correctly operate with :short variable" do
      gvar_test("s16", :short, i)
    end
  end

  [ 0, 0x7fff, 0x8000, 0xffff ].each do |i|
    it "should correctly operate with :ushort variable" do
      gvar_test("u16", :ushort, i)
    end
  end

  [ 0, 0x7fffffff, -0x80000000, -1 ].each do |i|
    it "should correctly operate with :int variable" do
      gvar_test("s32", :int, i)
    end
  end

  [ 0, 0x7fffffff, 0x80000000, 0xffffffff ].each do |i|
    it "should correctly operate with :uint variable" do
      gvar_test("u32", :uint, i)
    end
  end

  [ 0, 0x7fffffffffffffff, -0x8000000000000000, -1 ].each do |i|
    it "should correctly operate with :long_long variable" do
      gvar_test("s64", :long_long, i)
    end
  end

  [ 0, 0x7fffffffffffffff, 0x8000000000000000, 0xffffffffffffffff ].each do |i|
    it "should correctly operate with :ulong_long variable" do
      gvar_test("u64", :ulong_long, i)
    end
  end

  if FFI::Platform::LONG_SIZE == 32
    [ 0, 0x7fffffff, -0x80000000, -1 ].each do |i|
      it "should correctly operate with :long variable" do
        gvar_test("long", :long, i)
      end
    end

    [ 0, 0x7fffffff, 0x80000000, 0xffffffff ].each do |i|
      it "should correctly operate with :ulong variable" do
        gvar_test("ulong", :ulong, i)
      end
    end
  else
    [ 0, 0x7fffffffffffffff, -0x8000000000000000, -1 ].each do |i|
      it "should correctly operate with :long variable" do
        gvar_test("long", :long, i)
      end
    end

    [ 0, 0x7fffffffffffffff, 0x8000000000000000, 0xffffffffffffffff ].each do |i|
      it "should correctly operate with :ulong variable" do
        gvar_test("ulong", :ulong, i)
      end
    end
  end

  def gvar_lib(name, type)
    extended_module FFISpecs::LIBRARY do
      attach_variable :gvar, "gvar_#{name}", type
      attach_function :get, "gvar_#{name}_get", [], type
      attach_function :set, "gvar_#{name}_set", [ type ], :void
    end
  end

  def gvar_test(name, type, val)
    lib = gvar_lib(name, type)
    lib.set(val)
    lib.gvar.should == val
    lib.set(0)
    lib.gvar = val
    lib.get.should == val
  end
end
