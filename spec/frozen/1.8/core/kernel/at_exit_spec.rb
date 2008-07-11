require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Kernel.at_exit" do
  it "is a private method" do
    Kernel.private_instance_methods.should include("at_exit")
  end

  # TODO: create a helper for invoking a subprocess of the 
  # same implementation for this type of spec
  #result = `#{RUBY_CLI} -e "at_exit {print 5}; print 6"`
  #result.should == "65"
  it "runs after all other code"

  # TODO: create a helper for invoking a subprocess of the 
  # same implementation for this type of spec
  #result = `#{RUBY_CLI} -e "at_exit {print 4};at_exit {print 5}; print 6; at_exit {print 7}"`
  #result.should == '6754'
  it "runs in reverse order of registration"
end

describe "Kernel#at_exit" do
  it "needs to be reviewed for spec completeness"
end
