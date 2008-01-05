require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Hash#clear" do
  it "removes all key, value pairs" do
    h = { 1 => 2, 3 => 4 }
    h.clear.equal?(h).should == true
    h.should == {}
  end

  it "does not remove default values and procs" do
    h = Hash.new(5)
    h.clear
    h.default.should == 5

    h = Hash.new { 5 }
    h.clear
    h.default_proc.should_not == nil
  end

  compliant_on :ruby, :jruby do
    it "raises a TypeError if called on a frozen instance" do
      lambda { HashSpecs.frozen_hash.clear  }.should raise_error(TypeError)
      lambda { HashSpecs.empty_frozen_hash.clear }.should raise_error(TypeError)
    end
  end
end
