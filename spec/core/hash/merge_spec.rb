require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'
require File.dirname(__FILE__) + '/iteration_spec'
require File.dirname(__FILE__) + '/update_spec'

describe "Hash#merge" do
  it "returns a new hash by combining self with the contents of other" do
    { 1 => :a, 2 => :b, 3 => :c }.merge(:a => 1, :c => 2).should == { :c => 2, 1 => :a, 2 => :b, :a => 1, 3 => :c }
  end
  
  it "sets any duplicate key to the value of block if passed a block" do
    h1 = { :a => 2, :b => 1, :d => 5}
    h2 = { :a => -2, :b => 4, :c => -3 }
    r = h1.merge(h2) { |k,x,y| nil }
    r.should == { :a => nil, :b => nil, :c => -3, :d => 5 }
      
    r = h1.merge(h2) { |k,x,y| "#{k}:#{x+2*y}" }
    r.should == { :a => "a:-2", :b => "b:9", :c => -3, :d => 5 }

    should_raise(IndexError) do
      h1.merge(h2) { |k, x, y| raise(IndexError) }
    end

    r = h1.merge(h1) { |k,x,y| :x }
    r.should == { :a => :x, :b => :x, :d => :x }
  end
  
  it "calls to_hash on its argument" do
    obj = Object.new
    obj.should_receive(:to_hash, :returning => {1 => 2})
    {3 => 4}.merge(obj).should == {1 => 2, 3 => 4}
    
    obj = Object.new
    obj.should_receive(:respond_to?, :with => [:to_hash], :returning => true)
    obj.should_receive(:method_missing, :with => [:to_hash], :returning => { 1 => 2})
    {3 => 4}.merge(obj).should == {1 => 2, 3 => 4}
  end

  it "does not call to_hash on hash subclasses" do    
    {3 => 4}.merge(ToHashHash[1 => 2]).should == {1 => 2, 3 => 4}
  end

  it "returns subclass instance for subclasses" do
    MyHash[1 => 2, 3 => 4].merge({1 => 2}).class.should == MyHash
    MyHash[].merge({1 => 2}).class.should == MyHash

    {1 => 2, 3 => 4}.merge(MyHash[1 => 2]).class.should == Hash
    {}.merge(MyHash[1 => 2]).class.should == Hash
  end
  
  it "processes entries with same order as each()" do
    h = {1 => 2, 3 => 4, 5 => 6, "x" => nil, nil => 5, [] => []}
    merge_pairs = []
    each_pairs = []
    h.each { |pair| each_pairs << pair }
    h.merge(h) { |k, v1, v2| merge_pairs << [k, v1] }
    merge_pairs.should == each_pairs
  end

  it_behaves_like(@hash_iteration_method, :merge)
end

describe "Hash#merge!" do
  it_behaves_like(@hash_update, :merge!)

  it_behaves_like(@hash_iteration_method, :merge!)
end
