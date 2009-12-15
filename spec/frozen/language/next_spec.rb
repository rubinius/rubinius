require File.dirname(__FILE__) + '/../spec_helper'

class NextSpecs
  def self.yielding_method(expected)
    yield.should == expected
    :method_return_value
  end
end

describe "The next statement from within the block" do
  it "ends block execution" do
    a = []
    lambda {
      a << 1
      next
      a << 2
    }.call
    a.should == [1]
  end

  it "causes block to return nil if invoked without arguments" do
    lambda { 123; next; 456 }.call.should == nil
  end

  it "causes block to return nil if invoked with an empty expression" do
    lambda { next (); 456 }.call.should be_nil
  end

  it "returns the argument passed" do
    lambda { 123; next 234; 345 }.call.should == 234
  end

  it "returns to the invoking method" do
    NextSpecs.yielding_method(nil) { next }.should == :method_return_value
  end

  it "returns to the invoking method, with the specified value" do
    NextSpecs.yielding_method(nil) {
      next nil;
      fail("next didn't end the block execution")
    }.should == :method_return_value

    NextSpecs.yielding_method(1) {
      next 1
      fail("next didn't end the block execution")
    }.should == :method_return_value

    NextSpecs.yielding_method([1, 2, 3]) {
      next 1, 2, 3
      fail("next didn't end the block execution")
    }.should == :method_return_value
  end

  it "returns to the currently yielding method in case of chained calls" do
    class ChainedNextTest
      def self.meth_with_yield(&b)
        yield.should == :next_return_value
        :method_return_value
      end
      def self.invoking_method(&b)
        meth_with_yield(&b)
      end
      def self.enclosing_method
        invoking_method do
          next :next_return_value
          :wrong_return_value
        end
      end
    end

    ChainedNextTest.enclosing_method.should == :method_return_value
  end

end

describe "Assignment via next" do
  it "assigns objects" do
    def r(val); a = yield(); val.should == a; end
    r(nil){next}
    r(nil){next nil}
    r(1){next 1}
    r([]){next []}
    r([1]){next [1]}
    r([nil]){next [nil]}
    r([[]]){next [[]]}
    r([]){next [*[]]}
    r([1]){next [*[1]]}
    r([1,2]){next [*[1,2]]}
  end

  ruby_version_is ""..."1.9" do
    it "assigns splatted objects" do
      def r(val); a = yield(); val.should == a; end
      r(nil){next *nil}
      r(1){next *1}
      r(nil){next *[]}
      r(1){next *[1]}
      r(nil){next *[nil]}
      r([]){next *[[]]}
      r(nil){next *[*[]]}
      r(1){next *[*[1]]}
      r([1,2]){next *[*[1,2]]}
    end
  end

  ruby_version_is "1.9" do
    it "assigns splatted objects" do
      def r(val); a = yield(); val.should == a; end
      r([]){next *nil}
      r([1]){next *1}
      r([]){next *[]}
      r([1]){next *[1]}
      r([nil]){next *[nil]}
      r([[]]){next *[[]]}
      r([]){next *[*[]]}
      r([1]){next *[*[1]]}
      r([1,2]){next *[*[1,2]]}
    end
  end

  ruby_version_is ""..."1.9" do
    it "assigns objects to a splatted reference" do
      def r(val); *a = yield(); val.should == a; end
      r([nil]){next}
      r([nil]){next nil}
      r([1]){next 1}
      r([[]]){next []}
      r([[1]]){next [1]}
      r([[nil]]){next [nil]}
      r([[[]]]){next [[]]}
      r([[1,2]]){next [1,2]}
      r([[]]){next [*[]]}
      r([[1]]){next [*[1]]}
      r([[1,2]]){next [*[1,2]]}
    end
  end

  ruby_version_is "1.9" do
    it "assigns objects to a splatted reference" do
      def r(val); *a = yield(); val.should == a; end
      r([nil]){next}
      r([nil]){next nil}
      r([1]){next 1}
      r([]){next []}
      r([1]){next [1]}
      r([nil]){next [nil]}
      r([[]]){next [[]]}
      r([1,2]){next [1,2]}
      r([]){next [*[]]}
      r([1]){next [*[1]]}
      r([1,2]){next [*[1,2]]}
    end
  end

  ruby_version_is ""..."1.9" do
    it "assigns splatted objects to a splatted reference via a splatted yield" do
      def r(val); *a = *yield(); val.should == a; end
      r([nil]){next *nil}
      r([1]){next *1}
      r([nil]){next *[]}
      r([1]){next *[1]}
      r([nil]){next *[nil]}
      r([]){next *[[]]}
      r([1,2]){next *[1,2]}
      r([nil]){next *[*[]]}
      r([1]){next *[*[1]]}
      r([1,2]){next *[*[1,2]]}
    end
  end

  ruby_version_is "1.9" do
    it "assigns splatted objects to a splatted reference via a splatted yield" do
      def r(val); *a = *yield(); val.should == a; end
      r([]){next *nil}
      r([1]){next *1}
      r([]){next *[]}
      r([1]){next *[1]}
      r([nil]){next *[nil]}
      r([[]]){next *[[]]}
      r([1,2]){next *[1,2]}
      r([]){next *[*[]]}
      r([1]){next *[*[1]]}
      r([1,2]){next *[*[1,2]]}
    end
  end

  it "assigns objects to multiple variables" do
    def r(val); a,b,*c = yield(); val.should == [a,b,c]; end
    r([nil,nil,[]]){next}
    r([nil,nil,[]]){next nil}
    r([1,nil,[]]){next 1}
    r([nil,nil,[]]){next []}
    r([1,nil,[]]){next [1]}
    r([nil,nil,[]]){next [nil]}
    r([[],nil,[]]){next [[]]}
    r([1,2,[]]){next [1,2]}
    r([nil,nil,[]]){next [*[]]}
    r([1,nil,[]]){next [*[1]]}
    r([1,2,[]]){next [*[1,2]]}
  end

  ruby_version_is ""..."1.9" do
    it "assigns splatted objects to multiple variables" do
      def r(val); a,b,*c = *yield(); val.should == [a,b,c]; end
      r([nil,nil,[]]){next *nil}
      r([1,nil,[]]){next *1}
      r([nil,nil,[]]){next *[]}
      r([1,nil,[]]){next *[1]}
      r([nil,nil,[]]){next *[nil]}
      r([nil,nil,[]]){next *[[]]}
      r([1,2,[]]){next *[1,2]}
      r([nil,nil,[]]){next *[*[]]}
      r([1,nil,[]]){next *[*[1]]}
      r([1,2,[]]){next *[*[1,2]]}
    end
  end

  ruby_version_is "1.9" do
    it "assigns splatted objects to multiple variables" do
      def r(val); a,b,*c = *yield(); val.should == [a,b,c]; end
      r([nil,nil,[]]){next *nil}
      r([1,nil,[]]){next *1}
      r([nil,nil,[]]){next *[]}
      r([1,nil,[]]){next *[1]}
      r([nil,nil,[]]){next *[nil]}
      r([[],nil,[]]){next *[[]]}
      r([1,2,[]]){next *[1,2]}
      r([nil,nil,[]]){next *[*[]]}
      r([1,nil,[]]){next *[*[1]]}
      r([1,2,[]]){next *[*[1,2]]}
    end
  end
end

language_version __FILE__, "next"
