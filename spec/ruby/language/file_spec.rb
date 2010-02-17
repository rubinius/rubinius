require File.dirname(__FILE__) + '/../spec_helper'
require File.dirname(__FILE__) + '/../fixtures/code_loading'
require File.dirname(__FILE__) + '/shared/__FILE__'

describe "The __FILE__ constant" do
  it "equals (eval) inside an eval" do
    eval("__FILE__").should == "(eval)"
  end
end

describe "The __FILE__ constant" do
  it_behaves_like :language___FILE__, :require, CodeLoadingSpecs::Method.new
end

describe "The __FILE__ constant" do
  it_behaves_like :language___FILE__, :require, Kernel
end

describe "The __FILE__ constant" do
  it_behaves_like :language___FILE__, :load, CodeLoadingSpecs::Method.new
end

describe "The __FILE__ constant" do
  it_behaves_like :language___FILE__, :load, Kernel
end
