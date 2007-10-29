require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Kernel#exec" do
  it "should raise a SystemCallError if cmd cannot execute" do
    should_raise(SystemCallError) { exec "" }
  end  
end
