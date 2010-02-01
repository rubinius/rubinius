require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/../../ruby/fixtures/common'
require File.dirname(__FILE__) + '/fixtures/common'
require File.dirname(__FILE__) + '/shared/load'

describe "Kernel#load" do
  it_behaves_like :rbx_kernel_load, :load, CodeLoadingSpecs::Method.new
end

describe "Kernel#load" do
  it_behaves_like :rbx_kernel_load_recursive, :load, CodeLoadingSpecs::Method.new
end

describe "Kernel#load" do
  it_behaves_like :rbx_kernel_load_no_ext, :load, CodeLoadingSpecs::Method.new
end

describe "Kernel#load" do
  it_behaves_like :rbx_kernel_load_rba_relative, :load, CodeLoadingSpecs::Method.new
end

describe "Kernel.load" do
  it_behaves_like :rbx_kernel_load, :load, Kernel
end

describe "Kernel.load" do
  it_behaves_like :rbx_kernel_load_recursive, :load, Kernel
end

describe "Kernel.load" do
  it_behaves_like :rbx_kernel_load_no_ext, :load, Kernel
end

describe "Kernel.load" do
  it_behaves_like :rbx_kernel_load_rba_relative, :load, Kernel
end
