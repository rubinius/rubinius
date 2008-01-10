require File.dirname(__FILE__) + '/../../spec_helper'
require 'enumerator'

describe "Enumerator.new" do
  it "creates a new custom enumerator with the given object, iterator and arguments" do
    enum = Enumerable::Enumerator.new(1, :upto, 3)
    enum.kind_of?(Enumerable::Enumerator).should == true
  end

  it "creates a new custom enumerator that responds to #each" do
    enum = Enumerable::Enumerator.new(1, :upto, 3)
    enum.respond_to?(:each).should == true
  end

  it "creates a new custom enumerator that runs correctly" do
    Enumerable::Enumerator.new(1, :upto, 3).map{|x|x}.should == [1,2,3]
  end
end
