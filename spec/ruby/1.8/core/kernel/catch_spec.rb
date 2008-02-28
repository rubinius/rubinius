require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Kernel#catch" do

  it "throws the given name and is caught by matching catch block" do
    bad = false
    catch :blah do
      throw :blah
      bad = true
    end
    bad.should == false
  end

  it "raises unless the thrown name is known" do
    proc {
      throw :blah
    }.should raise_error(NameError, "uncaught throw `blah'") { |error|
      error.name.should == :blah
    }
  end

  it "is a private method" do
    Kernel.private_instance_methods.should include("catch")
  end
end
