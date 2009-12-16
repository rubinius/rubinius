require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Array#uniq" do
  it "returns an array with no duplicates" do
    ["a", "a", "b", "b", "c"].uniq.should == ["a", "b", "c"]
  end

  ruby_bug "#", "1.8.6.277" do
    it "properly handles recursive arrays" do
      empty = ArraySpecs.empty_recursive_array
      empty.uniq.should == [empty]

      array = ArraySpecs.recursive_array
      array.uniq.should == [1, 'two', 3.0, array]
    end
  end

  it "uses eql? semantics" do
    [1.0, 1].uniq.should == [1.0, 1]
  end

  it "compares elements first with hash" do
    # Can't use should_receive because it uses hash internally
    x = mock('0')
    def x.hash() 0 end
    y = mock('0')
    def y.hash() 0 end

    [x, y].uniq.should == [x, y]
  end

  it "does not compare elements with different hash codes via eql?" do
    # Can't use should_receive because it uses hash and eql? internally
    x = mock('0')
    def x.eql?(o) raise("Shouldn't receive eql?") end
    y = mock('1')
    def y.eql?(o) raise("Shouldn't receive eql?") end

    def x.hash() 0 end
    def y.hash() 1 end

    [x, y].uniq.should == [x, y]
  end

  it "compares elements with matching hash codes with #eql?" do
    # Can't use should_receive because it uses hash and eql? internally
    a = Array.new(2) do
      obj = mock('0')

      def obj.hash()
        # It's undefined whether the impl does a[0].eql?(a[1]) or
        # a[1].eql?(a[0]) so we taint both.
        def self.eql?(o) taint; o.taint; false; end
        return 0
      end

      obj
    end

    a.uniq.should == a
    a[0].tainted?.should == true
    a[1].tainted?.should == true

    a = Array.new(2) do
      obj = mock('0')

      def obj.hash()
        # It's undefined whether the impl does a[0].eql?(a[1]) or
        # a[1].eql?(a[0]) so we taint both.
        def self.eql?(o) taint; o.taint; true; end
        return 0
      end

      obj
    end

    a.uniq.size.should == 1
    a[0].tainted?.should == true
    a[1].tainted?.should == true
  end

  it "returns subclass instance on Array subclasses" do
    ArraySpecs::MyArray[1, 2, 3].uniq.class.should == ArraySpecs::MyArray
  end
end

describe "Array#uniq!" do
  it "modifies the array in place" do
    a = [ "a", "a", "b", "b", "c" ]
    a.uniq!
    a.should == ["a", "b", "c"]
  end

  it "returns self" do
    a = [ "a", "a", "b", "b", "c" ]
    a.should equal(a.uniq!)
  end

  ruby_bug "#", "1.8.6.277" do
    it "properly handles recursive arrays" do
      empty = ArraySpecs.empty_recursive_array
      empty_dup = empty.dup
      empty.uniq!
      empty.should == empty_dup

      array = ArraySpecs.recursive_array
      expected = array[0..3]
      array.uniq!
      array.should == expected
    end
  end

  it "returns nil if no changes are made to the array" do
    [ "a", "b", "c" ].uniq!.should == nil
  end

  ruby_version_is "" ... "1.9" do
    it "raises a TypeError on a frozen array if modification would take place" do
      dup_ary = [1, 1, 2]
      dup_ary.freeze
      lambda { dup_ary.uniq! }.should raise_error(TypeError)
    end

    it "does not raise an exception on a frozen array if no modification takes place" do
      ArraySpecs.frozen_array.uniq!.should be_nil
    end
  end

  ruby_version_is "1.9" do
    ruby_bug "[ruby-core:23666]", "1.9.2" do
      it "raises a RuntimeError on a frozen array" do
        dup_ary = [1, 1, 2]
        dup_ary.freeze
        lambda { dup_ary.uniq! }.should raise_error(RuntimeError)
        lambda { ArraySpecs.frozen_array.uniq!}.should raise_error(RuntimeError)
      end
    end
  end

end
