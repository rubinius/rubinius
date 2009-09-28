require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'
require File.dirname(__FILE__) + '/shared/iteration'
require File.dirname(__FILE__) + '/shared/update'

describe "Hash#merge" do
  it "returns a new hash by combining self with the contents of other" do
    h = new_hash(1 => :a, 2 => :b, 3 => :c).merge(:a => 1, :c => 2)
    h.should == new_hash(:c => 2, 1 => :a, 2 => :b, :a => 1, 3 => :c)
  end

  it "sets any duplicate key to the value of block if passed a block" do
    h1 = new_hash(:a => 2, :b => 1, :d => 5)
    h2 = new_hash(:a => -2, :b => 4, :c => -3)
    r = h1.merge(h2) { |k,x,y| nil }
    r.should == new_hash(:a => nil, :b => nil, :c => -3, :d => 5)

    r = h1.merge(h2) { |k,x,y| "#{k}:#{x+2*y}" }
    r.should == new_hash(:a => "a:-2", :b => "b:9", :c => -3, :d => 5)

    lambda {
      h1.merge(h2) { |k, x, y| raise(IndexError) }
    }.should raise_error(IndexError)

    r = h1.merge(h1) { |k,x,y| :x }
    r.should == new_hash(:a => :x, :b => :x, :d => :x)
  end

  it "tries to convert the passed argument to a hash using #to_hash" do
    obj = mock('{1=>2}')
    obj.should_receive(:to_hash).and_return(new_hash(1 => 2))
    new_hash(3 => 4).merge(obj).should == new_hash(1 => 2, 3 => 4)
  end

  it "does not call to_hash on hash subclasses" do
    new_hash(3 => 4).merge(ToHashHash[1 => 2]).should == new_hash(1 => 2, 3 => 4)
  end

  it "returns subclass instance for subclasses" do
    MyHash[1 => 2, 3 => 4].merge(new_hash(1 => 2)).class.should == MyHash
    MyHash[].merge(new_hash(1 => 2)).class.should == MyHash

    new_hash(1 => 2, 3 => 4).merge(MyHash[1 => 2]).class.should == hash_class
    new_hash.merge(MyHash[1 => 2]).class.should == hash_class
  end

  it "processes entries with same order as each()" do
    h = new_hash(1 => 2, 3 => 4, 5 => 6, "x" => nil, nil => 5, [] => [])
    merge_pairs = []
    each_pairs = []
    h.each_pair { |k, v| each_pairs << [k, v] }
    h.merge(h) { |k, v1, v2| merge_pairs << [k, v1] }
    merge_pairs.should == each_pairs
  end

end

describe "Hash#merge!" do
  it_behaves_like(:hash_update, :merge!)

  # This bug is far too odd to explain in a comment; see
  # http://redmine.ruby-lang.org/issues/show/1535 for the closest I've got to
  # an explanation.
  ruby_bug "#1535", "1.8.7.174" do
    it "shouldn't raise spurious RuntimeErrors" do
      hash = {1 => 2, 3 => 4, 5 => 6}
      big_hash = {}
      64.times { |k| big_hash[k.to_s] = k }
      lambda{
        hash.each { hash.merge!(big_hash) }
      }.should_not raise_error(RuntimeError)
    end
  end
end
