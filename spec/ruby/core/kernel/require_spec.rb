require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/../../fixtures/common'
require File.dirname(__FILE__) + '/shared/require'

describe "Kernel#require" do
  # if this fails, update your rubygems
  it "is a private method" do
    Kernel.should have_private_instance_method(:require)
  end

  it_behaves_like :kernel_require_basic, :require, CodeLoadingSpecs::Method.new

  it_behaves_like :kernel_require, :require, CodeLoadingSpecs::Method.new
end

describe "Kernel.require" do
  it_behaves_like :kernel_require_basic, :require, Kernel

  it_behaves_like :kernel_require, :require, Kernel
end
