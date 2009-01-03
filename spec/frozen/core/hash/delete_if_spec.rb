require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'
require File.dirname(__FILE__) + '/shared/iteration'

describe "Hash#delete_if" do
  it "yields two arguments: key and value" do
    all_args = []
    {1 => 2, 3 => 4}.delete_if { |*args| all_args << args }
    all_args.should == [[1, 2], [3, 4]]
  end
  
  it "removes every entry for which block is true and returns self" do
    h = {:a => 1, :b => 2, :c => 3, :d => 4}
    h.delete_if { |k,v| v % 2 == 1 }.should equal(h)
    h.should == {:b => 2, :d => 4}
  end
  
  it "processes entries with the same order as each()" do
    h = {:a => 1, :b => 2, :c => 3, :d => 4}

    each_pairs = []
    delete_pairs = []

    h.each_pair { |*pair| each_pairs << pair }
    h.delete_if { |*pair| delete_pairs << pair }

    each_pairs.should == delete_pairs
  end

  compliant_on :ruby, :jruby do
    it "raises a TypeError if called on a frozen instance" do
      lambda { HashSpecs.frozen_hash.delete_if { false } }.should raise_error(TypeError)
      lambda { HashSpecs.empty_frozen_hash.delete_if { true } }.should raise_error(TypeError)
    end
  end
  
  it_behaves_like(:hash_iteration_method, :delete_if)
  it_behaves_like(:hash_iteration_no_block, :delete_if)
end
