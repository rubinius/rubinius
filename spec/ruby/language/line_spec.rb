require File.dirname(__FILE__) + '/../spec_helper'
require File.dirname(__FILE__) + '/../fixtures/common'
require File.dirname(__FILE__) + '/shared/__LINE__'

describe "The __LINE__ constant" do
  before :each do
    ScratchPad.record []
  end

  after :each do
    ScratchPad.clear
  end

  it "equals the line number of the text inside an eval" do
    eval <<-EOC
ScratchPad << __LINE__

# line 3

ScratchPad << __LINE__
    EOC

    ScratchPad.recorded.should == [1, 5]
  end

  it_behaves_like :language___LINE__, :require, CodeLoadingSpecs::RequireMethod.new

  it_behaves_like :language___LINE__, :require, Kernel

  it_behaves_like :language___LINE__, :load, CodeLoadingSpecs::LoadMethod.new

  it_behaves_like :language___LINE__, :load, Kernel
end
