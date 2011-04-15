require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

describe "Module#name" do
  ruby_version_is ""..."1.9" do
    it "returns an empty String by default" do
      Module.new.name.should == ""
      Class.new.name.should == ""
    end
  end

  ruby_version_is "1.9" do
    it "returns nil by default" do
      Module.new.name.should be_nil
      Class.new.name.should be_nil
    end

    require File.expand_path('../fixtures/name', __FILE__)
    # http://redmine.ruby-lang.org/issues/show/1833
    it "preserves the encoding in which the class was defined" do
      ModuleSpecs::NameEncoding.new.name.encoding.should == Encoding::UTF_8
    end
  end

  it "returns the name of self" do

    ModuleSpecs.name.should == "ModuleSpecs"
    ModuleSpecs::Child.name.should == "ModuleSpecs::Child"
    ModuleSpecs::Parent.name.should == "ModuleSpecs::Parent"
    ModuleSpecs::Basic.name.should == "ModuleSpecs::Basic"
    ModuleSpecs::Super.name.should == "ModuleSpecs::Super"

    begin
      (ModuleSpecs::X = Module.new).name.should == "ModuleSpecs::X"
    ensure
      ModuleSpecs.send :remove_const, :X
    end
  end
end
