require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Hash#to_a" do
  it "returns a list of [key, value] pairs with same order as each()" do
    h = new_hash(:a => 1, 1 => :a, 3 => :b, :b => 5)
    pairs = []

    h.each_pair do |*pair|
      pairs << pair
    end

    h.to_a.class.should == Array
    h.to_a.should == pairs
  end
end
