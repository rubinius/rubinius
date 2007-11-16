require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Kernel#callcc" do
  it "raises a LocalJumpError if callcc is not given a block" do
    should_raise(LocalJumpError) { Kernel.callcc }
  end
end
