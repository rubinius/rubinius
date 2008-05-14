require File.dirname(__FILE__) + '/../../spec_helper'

describe "Symbol#to_proc" do
  it "should create the correct method" do
    pr = (:dup).to_proc
    m = [[(1..5).to_a, (1..3).to_a],%w(a b c), []] # Some dummy array
    pr.call(m).should == m
  end

  it "should pass the argument given to the method that is going to be called" do
    one_hash = {:a => 1, :b => 2}
    other_hash = {:a => 2, :c => 5}
    final = one_hash.merge(other_hash)

    pr = (:merge).to_proc
    pr.call(one_hash, other_hash).should == final
  end

  it "should be able to handle arrays as arguments" do
    a = (1..10).to_a
    (:to_s).to_proc.call(a).should == a.to_s
  end

  it "should be able to also handle multi-dimensional arrays" do
    a = [(1..10).to_a, (1..10).to_a]
    (:to_s).to_proc.call(a).should == a.to_s
  end
end
