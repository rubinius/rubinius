require File.dirname(__FILE__) + '/../spec_helper'
require File.dirname(__FILE__) + '/subtend_helper'

compile_extension('subtend_array')
require File.dirname(__FILE__) + '/ext/subtend_array'

describe "SubtendArray" do
  before :each do
    @s = SubtendArray.new
  end

  it "rb_ary_new should return an empty array" do
    @s.new_array.should == []
  end

  it "rb_ary_push should add an element to an array" do
    @s.rb_ary_push([], 4).should == [4]
  end

  it "rb_ary_push2 should add elements to an array" do
    o = mock('x')
    @s.rb_ary_push2([], 4, o).should == [4,o]
  end

  it "rb_ary_pop should remove the last element in the array and return it" do
    a = [1,2,3]
    @s.rb_ary_pop(a).should == 3
    a.should == [1,2]
  end

  it "rb_ary_join should join elements of an array with a string" do
    a = [1,2,3]
    b = ","
    @s.rb_ary_join(a,b).should == "1,2,3"
  end

  it "rb_ary_reverse should reverse an array" do
    a = [1,2,3]
    @s.rb_ary_reverse(a).should == [3,2,1]
  end

  it "rb_ary_entry should return nil when called with an empty array" do
    @s.rb_ary_entry([], 0).should == nil
  end

  it "rb_ary_entry should wrap around the offset if it's negative" do
    @s.rb_ary_entry([1, 2, 3], -1).should == 3
    @s.rb_ary_entry([1, 2, 3], -2).should == 2
  end

  it "rb_ary_entry should return nil if the index is out of range" do
    @s.rb_ary_entry([1, 2, 3], 3).should == nil
    @s.rb_ary_entry([1, 2, 3], -10).should == nil
  end

  it "rb_ary_clear should remove all elements from the array" do
    @s.rb_ary_clear([]).should == []
    @s.rb_ary_clear([1, 2, 3]).should == []
  end

  it "rb_ary_dup should duplicate the array" do
    @s.rb_ary_dup([]).should == []

    a = [1, 2, 3]
    b = @s.rb_ary_dup(a)

    b.should == a
    b.should_not equal(a)
  end

  it "rb_ary_unshift should prepend the element to the array" do
    a = [1, 2, 3]
    @s.rb_ary_unshift(a, "a").should == ["a", 1, 2, 3]
    a.should == ['a', 1, 2, 3]
  end

  it "rb_ary_shift should remove and return the first element" do
    a = [5, 1, 1, 5, 4]
    @s.rb_ary_shift(a).should == 5
    a.should == [1, 1, 5, 4]
  end

  it "rb_ary_shift should return nil when the array is empty" do
    @s.rb_ary_shift([]).should == nil
  end

  it "rb_ary_store should overwrite the element at the given position" do
    a = [1, 2, 3]

    @s.rb_ary_store(a, 1, 5)

    a.should == [1, 5, 3]
  end

  it "rb_ary_store should wrap around the offset if it's negative" do
    a = [1, 2, 3]

    @s.rb_ary_store(a, -1, 5)

    a.should == [1, 2, 5]
  end

# HACK: Raise is implemented as a C++ exception which crashes this
#
#  it "rb_ary_store should raise IndexError if the offset is still negative after wrap around " do
#    a = [1, 2, 3]
#
#    lambda { @s.rb_ary_store(a, -10, 5) }.should raise_error(IndexError)
#  end

  it "rb_ary_store should enlarge the array if necessary" do
    a = []

    @s.rb_ary_store(a, 2, 7)

    a.should == [nil, nil, 7]
  end

  it "RARRAY(a)->ptr should access the elements of an array" do
    a = [1, 2, 3]
    b = []
    @s.rb_rarray_iterate(a) do |e|
      b << e
    end
    a.should == b
  end

  it "Changing RARRAY(a)->ptr should change the array" do
    $global_rarray_test = [1, 2, 3, 4, 5, 6, 7]
    @s.rb_rarray_assign_global_alphabet
    $global_rarray_test.should == ['a', 'b', 'c', 'd', 'e'];
  end

  it "Reducing RARRAY(a)->len should cut the array" do
    t = [1, 2, 3, 4, 5]
    @s.rb_rarray_set_len(t, 3)
    t.should == [1, 2, 3]
  end
end
