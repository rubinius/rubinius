require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

describe "Hash.[]" do
  it "creates a Hash; values can be provided as the argument list" do
    hash_class[:a, 1, :b, 2].should == new_hash(:a => 1, :b => 2)
    hash_class[].should == new_hash
    hash_class[:a, 1, :b, new_hash(:c => 2)].should ==
      new_hash(:a => 1, :b => new_hash(:c => 2))
  end

  it "creates a Hash; values can be provided as one single hash" do
    hash_class[:a => 1, :b => 2].should == new_hash(:a => 1, :b => 2)
    hash_class[new_hash(1 => 2, 3 => 4)].should == new_hash(1 => 2, 3 => 4)
    hash_class[new_hash].should == new_hash
  end

  ruby_version_is '1.8.7' do
    # Not officially documented yet, see http://redmine.ruby-lang.org/issues/show/1385
    ruby_bug "[ruby-core:21249]", "1.8.7.167" do
      it "creates a Hash; values can be provided as a list of value-pairs in an array" do
        hash_class[[[:a, 1], [:b, 2]]].should == new_hash(:a => 1, :b => 2)
        hash_class[[[:a, 1], [:b], 42, [:d, 2], [:e, 2, 3], []]].should == new_hash(:a => 1, :b => nil, :d => 2)
        obj = mock('x')
        def obj.to_ary() [:b, 2] end
        hash_class[[[:a, 1], obj]].should == new_hash(:a => 1, :b => 2)
      end
    end
  end
  
  it "raises an ArgumentError when passed an odd number of arguments" do
    lambda { hash_class[1, 2, 3] }.should raise_error(ArgumentError)
    lambda { hash_class[1, 2, new_hash(3 => 4)] }.should raise_error(ArgumentError)
  end

  ruby_version_is '1.8.7' do
    it "calls to_hash" do
      obj = mock('x')
      def obj.to_hash() new_hash(1 => 2, 3 => 4) end
      hash_class[obj].should == new_hash(1 => 2, 3 => 4)
    end
  end

  it "returns an instance of the class it's called on" do
    hash_class[MyHash[1, 2]].class.should == hash_class
    MyHash[hash_class[1, 2]].should be_kind_of(MyHash)
    MyHash[[1,2,3,4]].should be_kind_of(MyHash)    
  end
end
