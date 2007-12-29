require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Module#autoload" do
  it "registers the given filename to be loaded the first time that the Module with the given name is accessed" do
    begin
      $m = Module.new { autoload(:A, File.dirname(__FILE__) + "/fixtures/autoload.rb") }
      
      # $m::A is set in module_spec.autoload.rb
      $m.const_get(:A).should == "test"

      # I know this is bad...
      $m = Module.new { autoload(:A, File.dirname(__FILE__) + "/../module/fixtures/autoload.rb") }
      $m.const_get(:A).should == "test"

      # And this is no better...
      $m = Module.new { autoload(:A, File.dirname(__FILE__) + "/../module/../module/fixtures/autoload.rb") }
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
end
