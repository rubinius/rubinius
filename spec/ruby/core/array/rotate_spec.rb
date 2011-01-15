require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

ruby_version_is "1.9" do
  describe "Array#rotate" do
    it "returns a copy of the array whose first n elements is moved to the last" do
      a = [1, 2, 3, 4, 5].freeze
      a.rotate.should == [2, 3, 4, 5, 1]
      a.rotate(2).should == [3, 4, 5, 1, 2]
      a.rotate(-1).should == [5, 1, 2, 3, 4]
      a.rotate(13).should == [4, 5, 1, 2, 3]
    end

    it "returns a copy of the array when the length is one" do
      a = [1].freeze
      a.rotate.should == [1]
      a.rotate(2).should == [1]
    end

    it "returns an empty array when self is empty" do
      a = [].freeze
      a.rotate.should == []
      a.rotate(2).should == []
    end

    it "does not return self" do
      a = [1, 2, 3]
      a.rotate.should_not equal(a)
    end

    ruby_version_is "" ... "1.9.3" do
      it "returns subclass instance for Array subclasses" do
        ArraySpecs::MyArray[1, 2, 3].rotate.should be_kind_of(ArraySpecs::MyArray)
      end
    end

    ruby_version_is "1.9.3" do
      it "does not return subclass instance for Array subclasses" do
        ArraySpecs::MyArray[1, 2, 3].rotate.should be_kind_of(Array)
      end
    end
  end

  describe "Array#rotate!" do
    it "moves the first n elements to the last and returns self" do
      a = [1, 2, 3, 4, 5]
      a.rotate!.should == [2, 3, 4, 5, 1]
      a.should == [2, 3, 4, 5, 1]
      a = [1, 2, 3, 4, 5]
      a.rotate!(2).should == [3, 4, 5, 1, 2]
      a.should == [3, 4, 5, 1, 2]
      a = [1, 2, 3, 4, 5]
      a.rotate!(-1).should == [5, 1, 2, 3, 4]
      a.should == [5, 1, 2, 3, 4]
      a = [1, 2, 3, 4, 5]
      a.rotate!(13).should == [4, 5, 1, 2, 3]
      a.should == [4, 5, 1, 2, 3]
    end

    it "does nothing and returns self when the length is zero or one" do
      a = [1]
      a.rotate! == [1]
      a.should == [1]
      a = [1]
      a.rotate!(2).should == [1]
      a.should == [1]

      a = []
      a.rotate!.should == []
      a.should == []
      a = []
      a.rotate!(2).should == []
      a.should == []
    end

    it "returns self" do
      a = [1, 2, 3]
      a.rotate!.object_id.should == a.object_id
    end

    it "raises a RuntimeError on a frozen array" do
      lambda { ArraySpecs.frozen_array.rotate! }.should raise_error(RuntimeError)
    end
  end
end
