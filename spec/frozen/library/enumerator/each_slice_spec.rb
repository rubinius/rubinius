require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'
require 'enumerator'

describe "Enumerator#each_slice" do
  it "iterates the block for each slice of n elements" do
    a = []
    EnumSpecs::Numerous.new.each_slice(3) { |e| a << e }
    a.should == [[2, 5, 3], [6, 1, 4]]
  end  
end
