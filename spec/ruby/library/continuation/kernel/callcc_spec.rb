require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/../../../fixtures/kernel/callcc'
require File.dirname(__FILE__) + '/../../../shared/kernel/callcc'

with_feature :continuation_library do
  require 'continuation'

  describe "Kernel#callcc" do
    it_behaves_like :kernel_instance_callcc, :callcc

    it_behaves_like :kernel_callcc, :callcc, KernelSpecs::Method.new
  end

  describe "Kernel.callcc" do
    it_behaves_like :kernel_callcc, :callcc, Kernel
  end
end
