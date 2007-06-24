require File.dirname(__FILE__) + '/../spec_helper'

describe "The next statement" do
  it "raises LocalJumpError if used not within block or while/for loop" do
    def x; next; end
    should_raise(LocalJumpError) { x }
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

  it "accepts argument but returns nil from blocks" do
    lambda { 123; next 234; 345 }.call.should == nil
  end
end

describe "Assignment via next" do
  it "should assign objects" do
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
  
  it "should assign splatted objects" do
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
  
  it "should assign objects to a splatted reference" do
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
  
  it "should assign splatted objects to a splatted reference via a splatted yield" do
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
  
  it "should assign objects to multiple variables" do
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

  it "should assign splatted objects to multiple variables" do
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
