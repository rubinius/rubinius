require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'
require 'enumerator'

describe "Enumerator#enum_slice" do
  it "returns an enumerator of the receiver with iteration of each_slice for each slice of n elements" do
    a = []
    enum = EnumSpecs::Numerous.new.enum_slice(4)
    enum.kind_of?(Enumerable::Enumerator).should == true
    enum.each { |e| a << e }
    a.should == [[2, 5, 3, 6], [1, 4]]
  end  
end
