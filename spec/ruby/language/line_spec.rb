require File.dirname(__FILE__) + '/../spec_helper'
require File.dirname(__FILE__) + '/../fixtures/code_loading'
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
end

describe "The __LINE__ constant" do
  it_behaves_like :language___LINE__, :require, CodeLoadingSpecs::Method.new
end

describe "The __LINE__ constant" do
  it_behaves_like :language___LINE__, :require, Kernel
end

describe "The __LINE__ constant" do
  it_behaves_like :language___LINE__, :load, CodeLoadingSpecs::Method.new
end

describe "The __LINE__ constant" do
  it_behaves_like :language___LINE__, :load, Kernel
end
