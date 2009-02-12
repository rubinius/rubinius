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

  it "should get passed whatever args were passed to Hash#new" do
    NewHash.new(:one, :two)[0].should == :one
    NewHash.new(:one, :two)[1].should == :two
  end

  it "raises a TypeError if called on a frozen instance" do
    block = lambda { HashSpecs.frozen_hash.instance_eval { initialize() }}
    block.should raise_error(TypeError)

    block = lambda { HashSpecs.frozen_hash.instance_eval { initialize(nil) }  }
    block.should raise_error(TypeError)

    block = lambda { HashSpecs.frozen_hash.instance_eval { initialize(5) }    }
    block.should raise_error(TypeError)

    block = lambda { HashSpecs.frozen_hash.instance_eval { initialize { 5 } } }
    block.should raise_error(TypeError)
  end
end
