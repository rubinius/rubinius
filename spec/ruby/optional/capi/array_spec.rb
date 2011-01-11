require File.expand_path('../spec_helper', __FILE__)

load_extension("array")

describe "C-API Array function" do
  before :each do
    @s = CApiArraySpecs.new
  end

  ruby_version_is ""..."1.9" do
    describe "RARRAY" do
      it "is sync'd with the object properly" do
        a2 = [1]

        ary = [:foo]

        @s.RARRAY_len(ary).should == 1
        ary[0].should == :foo

        ary[0] = :bar
        @s.RARRAY_len(a2).should == 1
        ary[0].should == :bar
      end
    end
  end

  describe "rb_ary_new" do
    it "returns an empty array" do
      @s.rb_ary_new.should == []
    end
  end

  describe "rb_ary_new2" do
    it "returns an empty array" do
      @s.rb_ary_new2(5).should == []
    end

    ruby_version_is ""..."1.9" do
      it "returns an array which can be assigned to from C" do
        ary = @s.rb_ary_new2(5)
        @s.rb_ary_new2_assign(ary, :set, 5)
        ary.should == [:set] * 5
      end
    end
  end

  describe "rb_ary_new3" do
    it "returns an array with the passed cardinality and varargs" do
      @s.rb_ary_new3(1,2,3).should == [1,2,3]
    end
  end

  describe "rb_ary_new4" do
    it "returns returns an array with the passed values" do
      @s.rb_ary_new4(1,2,3).should == [1,2,3]
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

  describe "rb_ary_to_s" do
    ruby_version_is ""..."1.9" do
      it "joins elements of an array with a string" do
        @s.rb_ary_to_s([1,2,3]).should == "123"
        @s.rb_ary_to_s([]).should == ""
      end
    end

    ruby_version_is "1.9" do
      it "creates an Array literal representation as a String" do
        @s.rb_ary_to_s([1,2,3]).should == "[1, 2, 3]"
        @s.rb_ary_to_s([]).should == "[]"
      end
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

    it "returns elements from the end when passed a negative index" do
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

  ruby_version_is ""..."1.9" do
    describe "RARRAY" do
      it "returns a struct with a pointer to a C array of the array's elements" do
        a = [1, 2, 3]
        b = []
        @s.RARRAY_ptr_iterate(a) do |e|
          b << e
        end
        a.should == b
      end

      it "allows assigning to the elements of the C array" do
        a = [1, 2, 3]
        @s.RARRAY_ptr_assign(a, :nasty)
        a.should == [:nasty, :nasty, :nasty]
      end

      it "allows changing the array and calling an rb_ary_xxx function" do
        a = [1, 2, 3]
        @s.RARRAY_ptr_assign_call(a)
        a.should == [1, 5, 7, 9]
      end

      it "allows changing the array and calling a method via rb_funcall" do
        a = [1, 2, 3]
        @s.RARRAY_ptr_assign_funcall(a)
        a.should == [1, 1, 2, 3]
      end

      it "returns a struct with the length of the array" do
        @s.RARRAY_len([1, 2, 3]).should == 3
      end

      it "is sync'd with the ruby Array object" do
        ary = Array.new(1000)

        @s.RARRAY_len(ary).should == 1000
        ary.clear  # shrink the array.

        @s.RARRAY_len(ary).should == 0

        # This extra check is to be sure that if there is a handle for
        # the ruby object it is updated.
        ary.size.should == 0

        # Now check that it can sync growing too
        1000.times { ary << 1 }

        @s.RARRAY_len(ary).should == 1000

        # Again, check that the possible handle doesn't confuse or misupdate
        # the ruby object.
        ary.size == 1000
      end
    end
  end

  describe "RARRAY_PTR" do
    it "returns a pointer to a C array of the array's elements" do
      a = [1, 2, 3]
      b = []
      @s.RARRAY_PTR_iterate(a) do |e|
        b << e
      end
      a.should == b
    end

    it "allows assigning to the elements of the C array" do
      a = [1, 2, 3]
      @s.RARRAY_PTR_assign(a, :set)
      a.should == [:set, :set, :set]
    end
  end

  describe "RARRAY_LEN" do
    it "returns the size of the array" do
      @s.RARRAY_LEN([1, 2, 3]).should == 3
    end
  end

  describe "rb_assoc_new" do
    it "returns an array containing the two elements" do
      @s.rb_assoc_new(1, 2).should == [1, 2]
      @s.rb_assoc_new(:h, [:a, :b]).should == [:h, [:a, :b]]
    end
  end

  describe "rb_ary_includes" do
    it "returns true if the array includes the element" do
      @s.rb_ary_includes([1, 2, 3], 2).should be_true
    end

    it "returns false if the array does not include the element" do
      @s.rb_ary_includes([1, 2, 3], 4).should be_false
    end
  end

  describe "rb_ary_aref" do
    it "returns the element at the given index" do
      @s.rb_ary_aref([:me, :you], 0).should == :me
      @s.rb_ary_aref([:me, :you], 1).should == :you
    end

    it "returns nil for an out of range index" do
      @s.rb_ary_aref([1, 2, 3], 6).should be_nil
    end

    it "returns a new array where the first argument is the index and the second is the length" do
      @s.rb_ary_aref([1, 2, 3, 4], 0, 2).should == [1, 2]
      @s.rb_ary_aref([1, 2, 3, 4], -4, 3).should == [1, 2, 3]
    end

    it "accepts a range" do
      @s.rb_ary_aref([1, 2, 3, 4], 0..-1).should == [1, 2, 3, 4]
    end

    it "returns nil when the start of a range is out of bounds" do
      @s.rb_ary_aref([1, 2, 3, 4], 6..10).should be_nil
    end

    it "returns an empty array when the start of a range equals the last element" do
      @s.rb_ary_aref([1, 2, 3, 4], 4..10).should == []
    end
  end

  describe "rb_iterate" do
    it "calls an callback function as a block passed to an method" do
      s = [1,2,3,4]
      s2 = @s.rb_iterate(s)

      s2.should == s

      # Make sure they're different objects
      s2.equal?(s).should be_false
    end

    it "calls a function with the other function available as a block" do
      h = {:a => 1, :b => 2}

      @s.rb_iterate_each_pair(h).sort.should == [1,2]
    end
  end

  describe "rb_ary_delete" do
    it "removes an element from an array and returns it" do
      ary = [1, 2, 3, 4]
      @s.rb_ary_delete(ary, 3).should == 3
      ary.should == [1, 2, 4]
    end

    it "returns nil if the element is not in the array" do
      ary = [1, 2, 3, 4]
      @s.rb_ary_delete(ary, 5).should be_nil
      ary.should == [1, 2, 3, 4]
    end
  end

  describe "rb_mem_clear" do
    it "sets elements of a C array to nil" do
      @s.rb_mem_clear(1).should == nil
    end
  end

  ruby_version_is ""..."1.9" do
    describe "rb_protect_inspect" do
      it "tracks an object recursively" do
        @s.rb_protect_inspect("blah").should be_true
      end
    end
  end

  describe "rb_ary_freeze" do
    it "freezes the object exactly like Object#freeze" do
      ary = [1,2]
      @s.rb_ary_freeze(ary)
      ary.frozen?.should be_true
    end
  end

  describe "rb_ary_delete_at" do
    before :each do
      @array = [1, 2, 3, 4]
    end

    it "removes an element from an array at a positive index" do
      @s.rb_ary_delete_at(@array, 2).should == 3
      @array.should == [1, 2, 4]
    end

    it "removes an element from an array at a negative index" do
      @s.rb_ary_delete_at(@array, -3).should == 2
      @array.should == [1, 3, 4]
    end

    it "returns nil if the index is out of bounds" do
      @s.rb_ary_delete_at(@array, 4).should be_nil
      @array.should == [1, 2, 3, 4]
    end

    it "returns nil if the negative index is out of bounds" do
      @s.rb_ary_delete_at(@array, -5).should be_nil
      @array.should == [1, 2, 3, 4]
    end
  end
end
