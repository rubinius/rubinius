require File.dirname(__FILE__) + '/../spec_helper'
require File.dirname(__FILE__) + '/../fixtures/common'
require File.dirname(__FILE__) + '/shared/__FILE__'

describe "The __FILE__ constant" do
  it "equals (eval) inside an eval" do
    eval("__FILE__").should == "(eval)"
  end

  it_behaves_like :language___FILE__, :require, CodeLoadingSpecs::RequireMethod.new

  it_behaves_like :language___FILE__, :require, Kernel

  it_behaves_like :language___FILE__, :load, CodeLoadingSpecs::LoadMethod.new

  it_behaves_like :language___FILE__, :load, Kernel
end
