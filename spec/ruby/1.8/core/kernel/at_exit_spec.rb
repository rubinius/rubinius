require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Kernel.at_exit" do
  it "runs after all other code" do
    result = `#{RUBY_CLI} -e "at_exit {print 5}; print 6"`
    result.should == "65"
  end

  it "runs in reverse order of registration" do
    result = `#{RUBY_CLI} -e "at_exit {print 4};at_exit {print 5}; print 6; at_exit {print 7}"`
    result.should == '6754'
  end
end
