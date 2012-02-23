require File.expand_path('../../spec_helper', __FILE__)
require File.expand_path('../fixtures/module', __FILE__)

describe "The module keyword" do
  it "creates a new module with a non-qualified constant name" do
    module ModuleSpecsToplevel; end
    ModuleSpecsToplevel.should be_an_instance_of(Module)
  end

  it "creates a new module with a qualified constant name" do
    module ModuleSpecs::Nested; end
    ModuleSpecs::Nested.should be_an_instance_of(Module)
  end

  it "creates a new module with a variable qualified constant name" do
    m = Module.new
    module m::N; end
    m::N.should be_an_instance_of(Module)
  end

  it "reopens an existing module" do
    module ModuleSpecs; Reopened = true; end
    ModuleSpecs::Reopened.should be_true
  end

  it "reopens a module included in Object" do
    module IncludedModuleSpecs; Reopened = true; end
    ModuleSpecs::IncludedInObject::IncludedModuleSpecs::Reopened.should be_true
  end

  it "raises a TypeError if the constant is a Class" do
    lambda do
      module ModuleSpecs::Modules::Klass; end
    end.should raise_error(TypeError)
  end

  it "raises a TypeError if the constant is a String" do
    lambda { module ModuleSpecs::Modules::A; end }.should raise_error(TypeError)
  end

  it "raises a TypeError if the constant is a Fixnum" do
    lambda { module ModuleSpecs::Modules::B; end }.should raise_error(TypeError)
  end

  it "raises a TypeError if the constant is nil" do
    lambda { module ModuleSpecs::Modules::C; end }.should raise_error(TypeError)
  end

  it "raises a TypeError if the constant is true" do
    lambda { module ModuleSpecs::Modules::D; end }.should raise_error(TypeError)
  end

  it "raises a TypeError if the constant is false" do
    lambda { module ModuleSpecs::Modules::D; end }.should raise_error(TypeError)
  end
end

describe "A Module's name" do
  ruby_version_is ""..."1.9" do
    it "is an empty string for an anonymous module" do
      Module.new.name.should == ""
    end

    it "is an empty string when assigning to a constant in an anonymous module" do
      m = Module.new
      m::N = Module.new
      m::N.name.should == ""
    end
  end

  ruby_version_is "1.9" do
    it "is nil for an anonymous module" do
      Module.new.name.should be_nil
    end

    it "is nil when assigned to a constant in an anonymous module" do
      m = Module.new
      m::N = Module.new
      m::N.name.should be_nil
    end
  end

  it "is set when opened with the module keyword" do
    ModuleSpecs.name.should == "ModuleSpecs"
  end

  it "is set when a nested module is opened with the module keyword" do
    ModuleSpecs::Anonymous.name.should == "ModuleSpecs::Anonymous"
  end

  it "is set when assigning to a constant" do
    m = Module.new
    ModuleSpecs::Anonymous::A = m
    m.name.should == "ModuleSpecs::Anonymous::A"
  end

  it "is not modified when assigning to a new constant after it has been accessed" do
    m = Module.new
    ModuleSpecs::Anonymous::B = m
    m.name.should == "ModuleSpecs::Anonymous::B"
    ModuleSpecs::Anonymous::C = m
    m.name.should == "ModuleSpecs::Anonymous::B"
  end

  ruby_version_is "1.9" do
    # TODO: http://bugs.ruby-lang.org/issues/6067

    it "is set with a conditional assignment to a constant" do
      module ModuleSpecs::Anonymous
        D ||= Module.new
      end
      ModuleSpecs::Anonymous::D.name.should == "ModuleSpecs::Anonymous::D"
    end
  end

  it "is set when the anonymous outer module name is set" do
    m = Module.new
    m::N = Module.new
    ModuleSpecs::Anonymous::E = m
    m::N.name.should == "ModuleSpecs::Anonymous::E::N"
  end
end
