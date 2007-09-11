require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Module#autoload" do
  it "registers the given filename to be loaded the first time that the Module with the given name is accessed" do
    begin
      $m = Module.new { autoload(:A, File.dirname(__FILE__) + "/fixtures/autoload.rb") }
      
      # $m::A is set in module_spec.autoload.rb
      $m.const_get(:A).should == "test"

      # I know this is bad...
      $m = Module.new { autoload(:A, File.dirname(__FILE__) + "/../core/fixtures/autoload.rb") }
      $m.const_get(:A).should == "test"

      # And this is no better...
      $m = Module.new { autoload(:A, File.dirname(__FILE__) + "/../core/../core/fixtures/autoload.rb") }
      $m.const_get(:A).should == "test"
    ensure
      $m = nil
    end
  end
  
  it "does not autoload when the specified constant was already set" do
    begin
      $m = Module.new { autoload(:A, File.dirname(__FILE__) + "/../core/../core/../core/fixtures/autoload.rb") }
      $m.const_set(:A, "testing!")
      $m.const_get(:A).should == "testing!"
    ensure
      $m = nil
    end
  end

  it "raises a NameError when an invalid constant name is given" do
    should_raise(NameError, "autoload must be constant name") do
      Module.new { autoload("no_constant", File.dirname(__FILE__) + "/fixtures/autoload.rb") }
    end

    should_raise(NameError, "autoload must be constant name") do
      Module.new { autoload("123invalid", File.dirname(__FILE__) + "/fixtures/autoload.rb") }
    end

    should_raise(NameError, "autoload must be constant name") do
      Module.new { autoload("This One is invalid, too!", File.dirname(__FILE__) + "/fixtures/autoload.rb") }
    end
  end
  
  it "raises an ArgumentError when an empty filename is given" do
    should_raise(ArgumentError, "empty file name") do
      Module.new { autoload("A", "") }
    end
  end
end
