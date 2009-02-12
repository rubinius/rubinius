require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Kernel#block_given?" do
  it "is a private method" do
    Kernel.should have_private_instance_method(:block_given?)
  end
  
  it "returns true if and only if a block is supplied" do
    KernelSpecs::BlockGiven::accept_block {}.should_not == false
    KernelSpecs::BlockGiven::accept_block_as_argument {}.should_not == false
    KernelSpecs::BlockGiven::accept_block.should_not == true
    KernelSpecs::BlockGiven::accept_block_as_argument.should_not == true
  end
end

describe "Kernel.block_given?" do
  it "needs to be reviewed for spec completeness"
end
