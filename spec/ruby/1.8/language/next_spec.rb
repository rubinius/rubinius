require File.dirname(__FILE__) + '/../spec_helper'

describe "The next statement" do
  it "raises a LocalJumpError if used not within block or while/for loop" do
    def bad_meth; next; end
    lambda { bad_meth }.should raise_error(LocalJumpError)
  end

  it "ends block execution if used within block" do
    a = []
    lambda {
      a << 1
      next
      a << 2
    }.call
    a.should == [1]
  end

  it "causes block to return nil" do
    lambda { 123; next; 456 }.call.should == nil
  end

  it "returns the argument passed" do
    lambda { 123; next 234; 345 }.call.should == 234
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
