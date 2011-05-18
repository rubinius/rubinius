require File.expand_path('../../spec_helper', __FILE__)

module LangModuleSpec
  module Sub1; end
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
end
