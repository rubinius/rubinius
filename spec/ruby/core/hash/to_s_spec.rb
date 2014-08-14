require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)
require File.expand_path('../shared/to_s', __FILE__)

describe "Hash#to_s" do
  it "returns a string by calling Hash#to_a and using Array#join with default separator" do
    h = new_hash(:fun => 'x', 1 => 3, nil => "ok", [] => :y)
    h.to_a.to_s.should == h.to_s
    old, $, = $,, ':'
    h.to_a.to_s.should == h.to_s
    $, = old
  end
end
