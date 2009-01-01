require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Kernel.at_exit" do
  it "is a private method" do
    Kernel.private_instance_methods.should include("at_exit")
  end

  it "runs after all other code" do
    ruby_exe("at_exit {print 5}; print 6").should == "65"
  end

  it "runs in reverse order of registration" do
    code = "at_exit {print 4};at_exit {print 5}; print 6; at_exit {print 7}"
    ruby_exe(code).should == "6754"
  end
end

describe "Kernel#at_exit" do
  it "needs to be reviewed for spec completeness"
end
