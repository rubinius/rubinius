require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Hash#shift" do
  it "removes a pair from hash and return it (same order as to_a)" do
    h = { :a => 1, :b => 2, "c" => 3, nil => 4, [] => 5 }
    pairs = h.to_a
    
    h.size.times do
      r = h.shift
      r.class.should == Array
      r.should == pairs.shift
      h.size.should == pairs.size
    end
    
    h.should == {}
    h.shift.should == nil
  end
  
  it "returns (computed) default for empty hashes" do
    Hash.new(5).shift.should == 5
    h = Hash.new { |*args| args }
    h.shift.should == [h, nil]
  end

  compliant :mri do
    it "raises TypeError if called on a frozen instance" do
      should_raise(TypeError) { @hash.shift }
      should_raise(TypeError) { @empty.shift }
    end
  end
end
