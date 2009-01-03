require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'
require 'enumerator'

describe "Enumerator#each_cons" do
  it "iterates the block for each array of n consecutive elements" do
    a = []
    EnumSpecs::Numerous.new.each_cons(4) { |e| a << e }
    a.should == [[2, 5, 3, 6], [5, 3, 6, 1], [3, 6, 1, 4]]
  end
end
