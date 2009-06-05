require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Array#flatten" do
  it "returns a one-dimensional flattening recursively" do
    [[[1, [2, 3]],[2, 3, [4, [4, [5, 5]], [1, 2, 3]]], [4]], []].flatten.should == [1, 2, 3, 2, 3, 4, 4, 5, 5, 1, 2, 3, 4]
  end

  ruby_version_is "1.8.7" do
    it "takes an optional argument that determines the level of recursion" do
      [ 1, 2, [3, [4, 5] ] ].flatten(1).should == [1, 2, 3, [4, 5]]
    end

    ruby_version_is ""..."1.9.2" do
      it "returns self when the level of recursion is 0" do
        a = [ 1, 2, [3, [4, 5] ] ]
        a.flatten(0).should equal(a)
      end
    end

    ruby_version_is "1.9.2" do
      it "returns dup when the level of recursion is 0" do
        a = [ 1, 2, [3, [4, 5] ] ]
        a.flatten(0).should == a
        a.flatten(0).should_not equal(a)
      end
    end

    it "ignores negative levels" do
      [ 1, 2, [ 3, 4, [5, 6] ] ].flatten(-1).should == [1, 2, 3, 4, 5, 6]
      [ 1, 2, [ 3, 4, [5, 6] ] ].flatten(-10).should == [1, 2, 3, 4, 5, 6]
    end

    it "tries to convert passed Objects to Integers using #to_int" do
      obj = mock("Converted to Integer")
      obj.should_receive(:to_int).and_return(1)

      [ 1, 2, [3, [4, 5] ] ].flatten(obj).should == [1, 2, 3, [4, 5]]
    end

    it "raises a TypeError when the passed Object can't be converted to an Integer" do
      obj = mock("Not converted")
      lambda { [ 1, 2, [3, [4, 5] ] ].flatten(obj) }.should raise_error(TypeError)
    end
  end

  it "does not call flatten on elements" do
    obj = mock('[1,2]')
    obj.should_not_receive(:flatten)
    [obj, obj].flatten.should == [obj, obj]

    obj = [5, 4]
    obj.should_not_receive(:flatten)
    [obj, obj].flatten.should == [5, 4, 5, 4]
  end

  it "raises an ArgumentError on recursive arrays" do
    x = []
    x << x
    lambda { x.flatten }.should raise_error(ArgumentError)

    x = []
    y = []
    x << y
    y << x
    lambda { x.flatten }.should raise_error(ArgumentError)
  end

  it "flattens any element which responds to #to_ary, using the return value of said method" do
    x = mock("[3,4]")
    x.should_receive(:to_ary).at_least(:once).and_return([3, 4])
    [1, 2, x, 5].flatten.should == [1, 2, 3, 4, 5]

    y = mock("MyArray[]")
    y.should_receive(:to_ary).at_least(:once).and_return(ArraySpecs::MyArray[])
    [y].flatten.should == []

    z = mock("[2,x,y,5]")
    z.should_receive(:to_ary).and_return([2, x, y, 5])
    [1, z, 6].flatten.should == [1, 2, 3, 4, 5, 6]
  end

  it "returns subclass instance for Array subclasses" do
    ArraySpecs::MyArray[].flatten.class.should == ArraySpecs::MyArray
    ArraySpecs::MyArray[1, 2, 3].flatten.class.should == ArraySpecs::MyArray
    ArraySpecs::MyArray[1, [2], 3].flatten.class.should == ArraySpecs::MyArray
    [ArraySpecs::MyArray[1, 2, 3]].flatten.class.should == Array
  end

  it "is not destructive" do
    ary = [1, [2, 3]]
    ary.flatten
    ary.should == [1, [2, 3]]
  end
end

describe "Array#flatten!" do
  it "modifies array to produce a one-dimensional flattening recursively" do
    a = [[[1, [2, 3]],[2, 3, [4, [4, [5, 5]], [1, 2, 3]]], [4]], []]
    a.flatten!
    a.should == [1, 2, 3, 2, 3, 4, 4, 5, 5, 1, 2, 3, 4]
  end

  it "returns self if made some modifications" do
    a = [[[1, [2, 3]],[2, 3, [4, [4, [5, 5]], [1, 2, 3]]], [4]], []]
    a.flatten!.should equal(a)
  end

  it "returns nil if no modifications took place" do
    a = [1, 2, 3]
    a.flatten!.should == nil
    a = [1, [2, 3]]
    a.flatten!.should_not == nil
  end

  ruby_version_is "1.8.7" do
    it "takes an optional argument that determines the level of recursion" do
      [ 1, 2, [3, [4, 5] ] ].flatten!(1).should == [1, 2, 3, [4, 5]]
    end

    ruby_bug "redmine #1440", "1.9.1" do
      it "returns nil when the level of recursion is 0" do
        a = [ 1, 2, [3, [4, 5] ] ]
        a.flatten!(0).should == nil
      end
    end

    it "treats negative levels as no arguments" do
      [ 1, 2, [ 3, 4, [5, 6] ] ].flatten!(-1).should == [1, 2, 3, 4, 5, 6]
      [ 1, 2, [ 3, 4, [5, 6] ] ].flatten!(-10).should == [1, 2, 3, 4, 5, 6]
    end

    it "tries to convert passed Objects to Integers using #to_int" do
      obj = mock("Converted to Integer")
      obj.should_receive(:to_int).and_return(1)

      [ 1, 2, [3, [4, 5] ] ].flatten!(obj).should == [1, 2, 3, [4, 5]]
    end

    it "raises a TypeError when the passed Object can't be converted to an Integer" do
      obj = mock("Not converted")
      lambda { [ 1, 2, [3, [4, 5] ] ].flatten!(obj) }.should raise_error(TypeError)
    end
  end

  it "does not call flatten! on elements" do
    obj = mock('[1,2]')
    obj.should_not_receive(:flatten!)
    [obj, obj].flatten!.should == nil

    obj = [5, 4]
    obj.should_not_receive(:flatten!)
    [obj, obj].flatten!.should == [5, 4, 5, 4]
  end

  it "raises an ArgumentError on recursive arrays" do
    x = []
    x << x
    lambda { x.flatten! }.should raise_error(ArgumentError)

    x = []
    y = []
    x << y
    y << x
    lambda { x.flatten! }.should raise_error(ArgumentError)
  end

  it "flattens any elements which responds to #to_ary, using the return value of said method" do
    x = mock("[3,4]")
    x.should_receive(:to_ary).at_least(:once).and_return([3, 4])
    [1, 2, x, 5].flatten!.should == [1, 2, 3, 4, 5]

    y = mock("MyArray[]")
    y.should_receive(:to_ary).at_least(:once).and_return(ArraySpecs::MyArray[])
    [y].flatten!.should == []

    z = mock("[2,x,y,5]")
    z.should_receive(:to_ary).and_return([2, x, y, 5])
    [1, z, 6].flatten!.should == [1, 2, 3, 4, 5, 6]

    ary = [ArraySpecs::MyArray[1, 2, 3]]
    ary.flatten!
    ary.class.should == Array
    ary.should == [1, 2, 3]
  end

  ruby_version_is '' ... '1.9' do
    it "raises a TypeError on frozen arrays when modification would take place" do
      nested_ary = [1, 2, []]
      nested_ary.freeze
      lambda { nested_ary.flatten! }.should raise_error(TypeError)
    end
  
    it "does not raise on frozen arrays when no modification would take place" do
      ArraySpecs.frozen_array.flatten!.should be_nil
    end
  end

  ruby_version_is '1.9' do
    ruby_bug "[ruby-core:23663]", "1.9.2" do
      it "raises a RuntimeError on frozen arrays" do
        nested_ary = [1, 2, []]
        nested_ary.freeze
        lambda { nested_ary.flatten! }.should raise_error(RuntimeError)
        lambda { ArraySpecs.frozen_array.flatten! }.should raise_error(RuntimeError)
      end
    end
  end
end
