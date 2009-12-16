require File.dirname(__FILE__) + '/../spec_helper'

module LangModuleSpec
  module Sub1; end
end

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
