require File.dirname(__FILE__) + '/../spec_helper'

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
