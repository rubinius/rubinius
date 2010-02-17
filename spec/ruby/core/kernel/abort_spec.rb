require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'
require File.dirname(__FILE__) + '/shared/abort'

describe "Kernel#abort" do
  it "is a private method" do
    Kernel.should have_private_instance_method(:abort)
  end

  it_behaves_like :kernel_abort, :abort, KernelSpecs::Method.new
end

describe "Kernel.abort" do
  it_behaves_like :kernel_abort, :abort, Kernel
end
