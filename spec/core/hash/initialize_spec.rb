require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Hash#initialize" do
  it "is private" do
    {}.private_methods.map { |m| m.to_s }.include?("initialize").should == true
  end

  it "can be used to reset default_proc" do
    h = { "foo" => 1, "bar" => 2 }
    h.default_proc.should == nil
    h.instance_eval { initialize { |h, k| k * 2 } }
    h.default_proc.should_not == nil
    h["a"].should == "aa"
  end

  compliant :mri do
    it "raises TypeError if called on a frozen instance" do
      @hash.instance_eval do
        lambda { initialize()     }.should raise_error(TypeError)
        lambda { initialize(nil)  }.should raise_error(TypeError)
        lambda { initialize(5)    }.should raise_error(TypeError)
        lambda { initialize { 5 } }.should raise_error(TypeError)
      end
    end
  end
end
