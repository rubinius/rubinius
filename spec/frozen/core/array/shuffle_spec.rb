require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Array#shuffle" do
  ruby_version_is "1.8.7" do
    it "should return the same values, in a usually different order" do
      a = [1,2,3,4]
      different = false
      10.times do
        s = a.shuffle
        s.sort.should == a
        different ||= (a != s)
      end
      different.should be_true # Will fail once in a blue moon (4!^10)
    end

    it "returns subclass instances with Array subclass" do
      ArraySpecs::MyArray[1, 2, 3].shuffle.class.should == ArraySpecs::MyArray
    end
  
    it "is not destructive" do
      a = [1, 2, 3]
      10.times do
        a.shuffle
        a.should == [1, 2, 3]
      end
    end

  end
end

describe "Array#shuffle!" do
  ruby_version_is "1.8.7" do
    it "should return the same values, in a usually different order" do
      a = [1,2,3,4]
      original = a
      different = false
      10.times do
        a = a.shuffle!
        a.sort.should == [1,2,3,4]
        different ||= (a != [1,2,3,4])
      end
      different.should be_true # Will fail once in a blue moon (4!^10)
      a.should equal(original)
    end

    ruby_version_is ""..."1.9" do
      it "raises a TypeError on a frozen array" do
        lambda { ArraySpecs.frozen_array.reverse! }.should raise_error(TypeError)
      end
    end

    ruby_version_is "1.9" do
      it "raises a RuntimeError on a frozen array" do
        lambda { ArraySpecs.frozen_array.shuffle! }.should raise_error(RuntimeError)
      end
    end
  end
end