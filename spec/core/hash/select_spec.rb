require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'
require File.dirname(__FILE__) + '/shared/iteration'

describe "Hash#select" do
  it "yields two arguments: key and value" do
    all_args = []
    {1 => 2, 3 => 4}.select { |*args| all_args << args }
    all_args.should == [[1, 2], [3, 4]]
  end
  
  it "returns an array of entries for which block is true" do
    { :a => 9, :c => 4, :b => 5, :d => 2 }.select { |k,v| v % 2 == 0 }
  end

  it "processes entries with the same order as reject" do
    h = { :a => 9, :c => 4, :b => 5, :d => 2 }
    
    select_pairs = []
    reject_pairs = []
    h.dup.select { |*pair| select_pairs << pair }
    h.reject { |*pair| reject_pairs << pair }
    
    select_pairs.should == reject_pairs
  end

  it_behaves_like(:hash_iteration_method, :select)
  it_behaves_like(:hash_iteration_modifying, :select)
  it_behaves_like(:hash_iteration_no_block, :select)
end
