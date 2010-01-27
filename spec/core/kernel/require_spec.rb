require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/../../ruby/fixtures/common'
require File.dirname(__FILE__) + '/fixtures/common'
require File.dirname(__FILE__) + '/shared/require'

describe "Kernel#require" do
  it_behaves_like :rbx_kernel_require, :require, CodeLoadingSpecs::Method.new
  it_behaves_like :rbx_kernel_require_recursive, :require, CodeLoadingSpecs::Method.new
  it_behaves_like :rbx_kernel_require_rba_relative, :require, CodeLoadingSpecs::Method.new
end

describe "Kernel.require" do
  it_behaves_like :rbx_kernel_require, :require, Kernel
  it_behaves_like :rbx_kernel_require_recursive, :require, Kernel
  it_behaves_like :rbx_kernel_require_rba_relative, :require, Kernel
end
