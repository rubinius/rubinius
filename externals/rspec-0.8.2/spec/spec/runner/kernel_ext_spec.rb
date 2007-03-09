require File.dirname(__FILE__) + '/../../spec_helper.rb'

module Spec
module Runner
context "KernelExt" do
  specify "should add context method to kernel" do
      lambda do
        context "" do
          
        end
      end.should_not_raise
    
  end
end
end
end