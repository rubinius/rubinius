require File.dirname(__FILE__) + '/../../spec_helper'

describe "MethodContext#stack_depth" do
  it "returns the depth of the bytecode operand stack" do
    ctx = Rubinius::Task.current.current_context
    ctx.stack_depth.should == 1
  end
end
