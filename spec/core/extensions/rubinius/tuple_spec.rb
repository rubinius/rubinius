require File.dirname(__FILE__) + '/../../../spec_helper'

class TupleElement
  def to_s
    'zonkers'
  end
  
  def stringify
    'bonkers'
  end
end

extension :rubinius do
  describe "Tuple.[]" do
    it "creates a new Tuple containing the elements provided" do
      t = Tuple[:one, 2, 'three']
      t[0].should == :one
      t[1].should == 2
      t[2].should == 'three'
    end
  end
  
  describe "Tuple#[]" do
    it "is an alias for #at" do
      t = Tuple[:one, :two]
      t[0].should == t.at(0)
    end
    
    it "raises InvalidIndex when index is greater than or equal to tuple size" do
      t = Tuple.new(1)
      lambda { t[1] }.should raise_error(InvalidIndex)
      lambda { t[5] }.should raise_error(InvalidIndex)
    end
    
    it "raises InvalidIndex when index is less than zero" do
      t = Tuple.new(1)
      lambda { t[-1] }.should raise_error(InvalidIndex)
    end
  end
  
  describe "Tuple#[]=" do
    it "is an alias for #put" do
    end
    
    it "raises InvalidIndex when index is greater than or equal to tuple size" do
      t = Tuple.new(1)
      lambda { t[1] = 'wrong' }.should raise_error(InvalidIndex)
      lambda { t[6] = 'wrong' }.should raise_error(InvalidIndex)
    end
    
    it "raises InvalidIndex when index is less than zero" do
      t = Tuple.new(1)
      lambda { t[-1] = 'wrong' }.should raise_error(InvalidIndex)
    end
  end
  
  describe "Tuple#at" do
    it "retrieves the element at the specified index" do
      t = Tuple.new(3)
      t.put(2, 'three')
      t.at(2).should == 'three'
    end

    it "raises InvalidIndex when index is greater than or equal to tuple size" do
      t = Tuple.new(1)
      lambda { t.at(1) }.should raise_error(InvalidIndex)
      lambda { t.at(5) }.should raise_error(InvalidIndex)
    end
  
    it "raises InvalidIndex when index is less than zero" do
      t = Tuple.new(1)
      lambda { t.at(-1) }.should raise_error(InvalidIndex)
    end
  end
  
  describe "Tuple#dup" do
    it "returns a copy of tuple" do
      t = Tuple[:a, 'three', 4].dup
      t[0].should == :a
      t[1].should == 'three'
      t[2].should == 4
      t.size.should == 3
    end
  end
  
  describe "Tuple#each" do
    it "passes each element to the block" do
      t = Tuple[:a, :c, :e, :g]
      ary = []
      t.each { |e| ary << e }
      ary.should == [:a, :c, :e, :g]
    end
    
    it "does nothing if tuple has zero length" do
      t = Tuple.new(0)
      ary = []
      t.each { |e| ary << e }
      ary.should == []
    end
  end
  
  describe "Tuple#first" do
    it "returns the first element" do
      t = Tuple[:foo, :bar, :baz]
      t.first.should == :foo
    end
  end
  
  describe "Tuple#last" do
    it "returns the last element" do
      t = Tuple[:a, 'b', 3]
      t.last.should == 3
    end
  end
  
  describe "Tuple#shift" do
    it "returns the tuple after removing the first value" do
      t = Tuple[99, 98, 97].shift
      t[0].should == 98
      t[1].should == 97
      t.size.should == 2
    end
    
    it "does not modify tuple if it has zero size" do
      t = Tuple.new(0)
      t.shift.should == t
    end
  end
  
  describe "Tuple#shifted" do
    it "returns a tuple with the specified number of elements added to the front" do
      t = Tuple[4, 5, 6].shifted(3)
      t[0].should == nil
      t[1].should == nil
      t[2].should == nil
      t[3].should == 4
      t[4].should == 5
      t[5].should == 6
    end
  end
  
  describe "Tuple#inspect" do
    it "returns a string representation" do
      Tuple[:a, 1, :b, 2].inspect.should == "#<Tuple: :a, 1, :b, 2>"
    end
  end
  
  describe "Tuple#join" do
    it "returns a string of the tuple elements separated by the separator string" do
      Tuple['a', :b, 2, '3'].join('-*-').should == "a-*-b-*-2-*-3"
    end
    
    it "defaults to calling to_s on the tuple elements" do
      t = Tuple[TupleElement.new, TupleElement.new]
      t.join(' ').should == "zonkers zonkers"
    end
    
    it "calls the specified method on the tuple elements" do
      t = Tuple[TupleElement.new, TupleElement.new]
      t.join(' ', :stringify).should == "bonkers bonkers"
    end
  end
  
  describe "Tuple#join_upto" do
    it "returns a string other specified number of tuple elements separated by the separator string" do
      Tuple[1, 2, 3, 4].join_upto(',', 2).should == "1,2"
    end
    
    it "returns an empty string if called on an empty tuple" do
      Tuple.new(0).join_upto(' ', 2).should == ""
    end
    
    it "joins all the elements if the upto index is greater than the number of elements" do
      Tuple[1, 2, 3].join_upto('', 6).should == "123"
    end
    
    it "defaults to calling to_s on the tuple elements" do
      t = Tuple[TupleElement.new, TupleElement.new]
      t.join_upto(' ', 1).should == "zonkers"
      t.join_upto('.', 0).should == ""
    end
    
    it "calls the specified method on the tuple elements" do
      t = Tuple[TupleElement.new, TupleElement.new]
      t.join_upto(' ', 1, :stringify).should == "bonkers"
      t.join_upto('.', 0, :stringify).should == ""
    end
  end

  describe "Tuple#new" do
    it "creates a Tuple of specified size" do
      Tuple.new(2).fields.should == 2
      Tuple.new(2).size.should == 2
      Tuple.new(2).length.should == 2
    end
  
    it "creates a Tuple of zero size" do
      Tuple.new(0).fields.should == 0
      Tuple.new(0).size.should == 0
      Tuple.new(0).length.should == 0
    end
  end
  
  describe "Tuple#put" do
    it "inserts an element at the specified index" do
      t = Tuple.new(1)
      t.put(0, "Whee")
      t.at(0).should == "Whee"
    end

    it "raises InvalidIndex when index is greater than or equal to tuple size" do
      t = Tuple.new(1)
      raise_error(InvalidIndex) { t.put(1,'wrong') }
    end
  
    it "raises InvalidIndex when index is less than zero" do
      t = Tuple.new(1)
      lambda { t.put(-1,'wrong') }.should raise_error(InvalidIndex)
    end
  end
  
  describe "Tuple#to_a" do
    it "returns an array containing the elements of the tuple" do
      Tuple[:a, 1, :b, 2].to_a == [:a, 1, :b, 2]
      Tuple.new(0).to_a == []
    end
  end
end
