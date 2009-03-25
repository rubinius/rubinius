require File.dirname(__FILE__) + '/spec_helper'

load_extension("array")

describe "C-API Array function" do
  before :each do
    @s = CApiArraySpecs.new
  end

  describe "rb_ary_new" do
    it "returns an empty array" do
      @s.rb_ary_new.should == []
    end
  end

  describe "rb_ary_push" do
    it "adds an element to the array" do
      @s.rb_ary_push([], 4).should == [4]
    end
  end

  describe "rb_ary_pop" do
    it "removes and returns the last element in the array" do
      a = [1,2,3]
      @s.rb_ary_pop(a).should == 3
      a.should == [1,2]
    end
  end

  describe "rb_ary_join" do
    it "joins elements of an array with a string" do
      a = [1,2,3]
      b = ","
      @s.rb_ary_join(a,b).should == "1,2,3"
    end
  end

  describe "rb_ary_reverse" do
    it "reverses the order of elements in the array" do
      a = [1,2,3]
      @s.rb_ary_reverse(a).should == [3,2,1]
    end
  end

  describe "rb_ary_entry" do
    it "returns nil when passed an empty array" do
      @s.rb_ary_entry([], 0).should == nil
    end

    it "returns elemends from the end when passed a negative index" do
      @s.rb_ary_entry([1, 2, 3], -1).should == 3
      @s.rb_ary_entry([1, 2, 3], -2).should == 2
    end

    it "returns nil if the index is out of range" do
      @s.rb_ary_entry([1, 2, 3], 3).should == nil
      @s.rb_ary_entry([1, 2, 3], -10).should == nil
    end
  end

  describe "rb_ary_clear" do
    it "removes all elements from the array" do
      @s.rb_ary_clear([]).should == []
      @s.rb_ary_clear([1, 2, 3]).should == []
    end
  end

  describe "rb_ary_dup" do
    it "duplicates the array" do
      @s.rb_ary_dup([]).should == []

      a = [1, 2, 3]
      b = @s.rb_ary_dup(a)

      b.should == a
      b.should_not equal(a)
    end
  end

  describe "rb_ary_unshift" do
    it "prepends the element to the array" do
      a = [1, 2, 3]
      @s.rb_ary_unshift(a, "a").should == ["a", 1, 2, 3]
      a.should == ['a', 1, 2, 3]
    end
  end

  describe "rb_ary_shift" do
    it "removes and returns the first element" do
      a = [5, 1, 1, 5, 4]
      @s.rb_ary_shift(a).should == 5
      a.should == [1, 1, 5, 4]
    end

    it "returns nil when the array is empty" do
      @s.rb_ary_shift([]).should == nil
    end
  end

  describe "rb_ary_store" do
    it "overwrites the element at the given position" do
      a = [1, 2, 3]
      @s.rb_ary_store(a, 1, 5)
      a.should == [1, 5, 3]
    end

    it "writes to elements offset from the end if passed a negative index" do
      a = [1, 2, 3]
      @s.rb_ary_store(a, -1, 5)
      a.should == [1, 2, 5]
    end

    it "raises on IndexError if the negative index is greater than the length" do
      a = [1, 2, 3]

      lambda { @s.rb_ary_store(a, -10, 5) }.should raise_error(IndexError)
    end

    it "enlarges the array as needed" do
      a = []
      @s.rb_ary_store(a, 2, 7)
      a.should == [nil, nil, 7]
    end
  end

  describe "RARRAY_PTR" do
    it "returns a pointer to a C array of the array's elements" do
      a = [1, 2, 3]
      b = []
      @s.rb_ary_iterate(a) do |e|
        b << e
      end
      a.should == b
    end
  end

  describe "RARRAY_LEN" do
    it "returns the size of the array" do
      a = [1, 2, 3]
      @s.RARRAY_LEN(a).should == 3
    end
  end
end
