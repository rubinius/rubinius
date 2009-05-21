require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'
require File.dirname(__FILE__) + '/shared/iteration'

describe "Hash#each" do
  it "yields one argument: [key, value]" do
    all_args = []
    new_hash(1 => 2, 3 => 4).each { |*args| all_args << args }
    all_args.sort.should == [[[1, 2]], [[3, 4]]]
  end

  it "calls block once for each entry, passing key, value" do
    r = new_hash
    h = new_hash(:a => 1, :b => 2, :c => 3, :d => 5)
    h.each { |k,v| r[k.to_s] = v.to_s }.should equal(h)
    r.should == new_hash("a" => "1", "b" => "2", "c" => "3", "d" => "5")
  end

  it "uses the same order as keys() and values()" do
    h = new_hash(:a => 1, :b => 2, :c => 3, :d => 5)
    keys = []
    values = []

    h.each do |k, v|
      keys << k
      values << v
    end

    keys.should == h.keys
    values.should == h.values
  end

  it_behaves_like(:hash_iteration_method, :each)
  it_behaves_like(:hash_iteration_modifying, :each)
  it_behaves_like(:hash_iteration_no_block, :each)
end
