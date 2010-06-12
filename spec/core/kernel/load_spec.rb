require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../../../ruby/fixtures/code_loading', __FILE__)
require File.expand_path('../../../fixtures/code_loading', __FILE__)
require File.expand_path('../shared/load', __FILE__)

describe "Kernel#load" do
  it_behaves_like :rbx_kernel_load, :load, CodeLoadingSpecs::Method.new
end

describe "Kernel#load" do
  it_behaves_like :rbx_kernel_load_no_ext, :load, CodeLoadingSpecs::Method.new
end

describe "Kernel.load" do
  it_behaves_like :rbx_kernel_load, :load, Kernel
end

describe "Kernel.load" do
  it_behaves_like :rbx_kernel_load_no_ext, :load, Kernel
end
