require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Module#autoload" do
  it "registers the given filename to be loaded the first time that the Module with the given name is accessed" do
    begin
      $m = Module.new { autoload(:AAA, File.dirname(__FILE__) + "/fixtures/autoload.rb") }

      # $m::AAA is set in module_spec.autoload.rb
      $m.const_get(:AAA).should == "test"

      # I know this is bad...
      $m = Module.new { autoload(:AAA, File.dirname(__FILE__) + "/../module/fixtures/autoload.rb") }
      $m.const_get(:AAA).should == "test"

      # And this is no better...
      $m = Module.new { autoload(:AAA, File.dirname(__FILE__) + "/../module/../module/fixtures/autoload.rb") }
      $m.const_get(:AAA).should == "test"
    ensure
      $m = nil
    end
  end

  it "does not autoload when the specified constant was already set" do
    begin
      $m = Module.new { autoload(:AAA, File.dirname(__FILE__) + "/../core/../core/../core/fixtures/autoload.rb") }
      $m.const_set(:AAA, "testing!")
      $m.const_get(:AAA).should == "testing!"
    ensure
      $m = nil
    end
  end

  it "raises a NameError when an invalid constant name is given" do
    lambda {
      Module.new { autoload("no_constant", File.dirname(__FILE__) + "/fixtures/autoload.rb") }
    }.should raise_error(NameError)

    lambda {
      Module.new { autoload("123invalid", File.dirname(__FILE__) + "/fixtures/autoload.rb") }
    }.should raise_error(NameError)

    lambda {
      Module.new { autoload("This One is invalid, too!", File.dirname(__FILE__) + "/fixtures/autoload.rb") }
    }.should raise_error(NameError)
  end
  
  it "raises an ArgumentError when an empty filename is given" do
    lambda { Module.new { autoload("A", "") } }.should raise_error(ArgumentError)
  end
  
  it "loads constants that are registered at toplevel" do
    ModuleSpecAutoloadToplevel.message.should == "success"
  end

  it "triggers an autoload before using a toplevel constant" do
    class ModuleSpecs::AutoLoadParent
      autoload(:AutoLoadSubject, File.dirname(__FILE__) + "/fixtures/autoload_nested.rb")
    end

    class ModuleSpecs::AutoLoadChild < ModuleSpecs::AutoLoadParent
      ModuleSpecs::AutoLoadParent::AutoLoadSubject.message.should == "success"
    end
  end

  it "should not fail when the load path is manually required" do
    module ModuleSpecs::AutoloadRequire
      fixture = File.dirname(__FILE__) + "/fixtures/autoload_require.rb"

      autoload(:ModuleSpecAutoloadRequire, fixture)
      require fixture
    end

    ModuleSpecs::AutoloadRequire::ModuleSpecAutoloadRequire.hello.should == "Hello, World!"
  end

  it "should correctly match paths with and without ruby file extensions" do
    module ModuleSpecs::AutoloadExtension
      with_rb = File.dirname(__FILE__) + "/fixtures/autoload_extension.rb"
      without_rb = with_rb[0..-4]

      autoload(:Constant, without_rb)
      require with_rb
    end
    ModuleSpecs::AutoloadExtension::Constant.message.should == "success"
  end
end

describe "Module#autoload?" do
  it "returns the name of the file that will be autoloaded" do
    m = Module.new { autoload :AAA, "module_a" }
    m.autoload?(:AAA).should == "module_a"
    m.autoload?(:B).should == nil
  end
end
