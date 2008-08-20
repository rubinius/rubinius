require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Hash#to_s" do
  it "returns a string by calling Hash#to_a and using Array#join with default separator" do
    h = { :fun => 'x', 1 => 3, nil => "ok", [] => :y }
    h.to_a.to_s.should == h.to_s
    old, $, = $,, ':'
    h.to_a.to_s.should == h.to_s
    $, = old
  end
end
