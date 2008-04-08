require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/../../bin/name_map'

module NameMapSpecs
  class A
    class B
    end
  end

  class Error
  end
end

describe NameMap, "#const_lookup" do
  before :each do
    @map = NameMap.new
  end

  it "returns the constant specified by 'A::B'" do
    @map.const_lookup("NameMapSpecs::A::B").should == NameMapSpecs::A::B
  end

  it "returns a regular constant specified without scoping" do
    @map.const_lookup("NameMapSpecs").should == NameMapSpecs
  end
end

describe NameMap, "#exception?" do
  before :each do
    @map = NameMap.new
  end

  it "returns true if the constant is Errno" do
    @map.exception?(Errno).should == true
  end

  it "returns true if the constant is a kind of Exception" do
    @map.exception?(Errno::EBADF).should == true
    @map.exception?(LoadError).should == true
    @map.exception?(SystemExit).should == true
  end

  it "returns false if the constant is not a kind of Exception" do
    @map.exception?(NameMapSpecs::Error).should == false
    @map.exception?(NameMapSpecs).should == false
  end
end

describe NameMap, "#get_class_or_module" do
  before :each do
    @map = NameMap.new
  end

  it "returns the constant specified by the string" do
    @map.get_class_or_module("NameMapSpecs").should == NameMapSpecs
  end

  it "returns the constant specified by the 'A::B' string" do
    @map.get_class_or_module("NameMapSpecs::A").should == NameMapSpecs::A
  end

  it "returns nil if the constant is not a class or module" do
    @map.get_class_or_module("Float::MAX").should == nil
  end

  it "returns nil if the constant is in the set of excluded constants" do
    @map.get_class_or_module("MkSpec").should == nil
    @map.get_class_or_module("DTracer").should == nil
    @map.get_class_or_module("NameMap").should == nil
    @map.get_class_or_module("OptionParser").should == nil
    @map.get_class_or_module("SystemExit").should == nil
  end

  it "returns nil if the constant does not exist" do
    @map.get_class_or_module("Heaven").should == nil
    @map.get_class_or_module("Hell").should == nil
    @map.get_class_or_module("Bush::Brain").should == nil
  end
end

describe NameMap, "#get_dir_name" do
  before :each do
    @map = NameMap.new
  end

  it "returns a directory name from the base name and constant" do
    @map.get_dir_name(NameMapSpecs, 'spec/core').should == 'spec/core/namemapspecs'
  end

  it "returns a directory name from the last component in the constants name" do
    @map.get_dir_name(NameMapSpecs::A, 'spec').should == 'spec/a'
  end

  it "returns a directory name without 'class' for constants like TrueClass" do
    @map.get_dir_name(TrueClass, 'spec').should == 'spec/true'
    @map.get_dir_name(FalseClass, 'spec').should == 'spec/false'
  end

  it "returns 'exception' for the directory name of any Exception subclass" do
    @map.get_dir_name(SystemExit, 'spec').should == 'spec/exception'
    @map.get_dir_name(Errno::EBADF, 'spec').should == 'spec/exception'
  end

  it "returns 'class' for Class" do
    @map.get_dir_name(Class, 'spec').should == 'spec/class'
  end
end

# These specs do not cover all the mappings, but only describe how the
# name is derived when the hash item maps to a single value, a hash with
# a specific item, or a hash with a :default item.
describe NameMap, "#get_spec_name" do
  before :each do
    @map = NameMap.new
  end

  it "returns the name of the spec file based on the constant and method" do
    @map.get_spec_name("[]=", "Array").should == "element_set_spec.rb"
  end

  it "returns the name of the spec file based on the special entry for the method" do
    @map.get_spec_name("~", "Regexp").should == "match_spec.rb"
    @map.get_spec_name("~", "Fixnum").should == "complement_spec.rb"
  end

  it "returns the name of the spec file based on the default entry for the method" do
    @map.get_spec_name("<<", "NameMapSpecs").should == "append_spec.rb"
  end
end
