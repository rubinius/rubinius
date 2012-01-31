require File.expand_path('../../spec_helper', __FILE__)

module LangModuleSpec
  module Sub1; end

  module AnonymousModules
    # used as a container for anonymous module specs testing const assignment
  end
end

module LangModuleSpecInObject
  module LangModuleTop
  end
end

# Must be here, we have to include it into Object because thats
# the case.
include LangModuleSpecInObject

module LangModuleSpec::Sub2; end

describe "module" do
  it "has the right name" do
    LangModuleSpec::Sub1.name.should == "LangModuleSpec::Sub1"
    LangModuleSpec::Sub2.name.should == "LangModuleSpec::Sub2"
  end

  it "gets a name when assigned to a constant" do
    LangModuleSpec::Anon = Module.new
    LangModuleSpec::Anon.name.should == "LangModuleSpec::Anon"
  end

  it "raises a TypeError if the constant is a class" do
    class LangModuleSpec::C1; end

    lambda {
      module LangModuleSpec::C1; end
    }.should raise_error(TypeError)
  end

  it "raises a TypeError if the constant is not a module" do
    module LangModuleSpec
      C2 = 2
    end

    lambda {
      module LangModuleSpec::C2; end
    }.should raise_error(TypeError)
  end

  it "allows for reopening a module subclass" do
    class ModuleSubClass < Module; end
    LangModuleSpec::C3 = ModuleSubClass.new

    module LangModuleSpec::C3
      C4 = 4
    end

    LangModuleSpec::C3::C4.should == 4
  end

  it "reopens a module included into Object" do
    module LangModuleTop
    end

    LangModuleTop.should == LangModuleSpecInObject::LangModuleTop
  end
end

describe "An anonymous module" do
  ruby_version_is "" ... "1.9" do
    it "returns an empty string for its name" do
      m = Module.new
      m.name.should == ""
    end
  end

  ruby_version_is "1.9" do
    it "returns nil for its name" do
      m = Module.new
      m.name.should == nil
    end
  end

  it "takes on the name of the first constant it is assigned to" do
    m1 = Module.new
    m1.inspect.should =~ /#<Module/
    LangModuleSpec::AnonymousModules::M1 = m1
    m1.name.should == "LangModuleSpec::AnonymousModules::M1"

    m2 = Module.new
    LangModuleSpec::AnonymousModules.const_set :M2, m2
    m2.name.should == "LangModuleSpec::AnonymousModules::M2"
  end

  it "forces named nested modules to be anonymous" do
    m3 = Module.new
    m3.const_set :M4, Module.new

    m3::M4.inspect.should =~ /#<Module/

    LangModuleSpec::AnonymousModules::M3 = m3
    m3::M4.name.should == "LangModuleSpec::AnonymousModules::M3::M4"

    m5 = Module.new
    m5.const_set :M6, Module.new

    LangModuleSpec::AnonymousModules.const_set :M5, m5
    m5::M6.name.should == "LangModuleSpec::AnonymousModules::M5::M6"
  end

  it "never recalculates full name once no longer anonymous" do
    m6 = Module.new
    m6.const_set :M7, Module.new
    LangModuleSpec::AnonymousModules::M6 = m6
    m6::M7.name.should == "LangModuleSpec::AnonymousModules::M6::M7"

    LangModuleSpec::AnonymousModules::M8 = m6::M7
    LangModuleSpec::AnonymousModules::M8.name.should == "LangModuleSpec::AnonymousModules::M6::M7"
  end
end
