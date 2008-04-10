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
      # The ruby docs are not clear whether NameError#name should
      # retrun String or Symbol. Well, the docs state the *String*
      # should be returned, but the actual MRI behavior is to return Symbol.
      # And in MRI 1.9, even different Exception raised altogether.

      # So, instead of checking that error.name == :blah, we perform
      # more generic test, suitable for different implementations
      # (like JRuby, since JRuby follows the ruby-doc, and returns String).
      error.name.to_s.should == "blah"
    }
  end

  it "is a private method" do
    Kernel.private_instance_methods.should include("catch")
  end
end

describe "Kernel.catch" do
  it "needs to be reviewed for spec completeness" do
  end
end
