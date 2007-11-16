require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Kernel#block_given?" do
  it "returns true if and only if a block is supplied" do
    KernelSpecs::BlockGiven::accept_block {}.should == true
    KernelSpecs::BlockGiven::accept_block_as_argument {}.should == true
    KernelSpecs::BlockGiven::accept_block.should == false
    KernelSpecs::BlockGiven::accept_block_as_argument.should == false
  end
end
