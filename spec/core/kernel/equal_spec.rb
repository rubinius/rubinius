require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Kernel#equal?" do
  it "returns true only if obj and other are the same object" do
    o1 = mock('o1')
    o2 = mock('o2')
    (o1.equal? o1).should == true
    (o2.equal? o2).should == true
    (o1.equal? o2).should== false
    (nil.equal? nil).should == true
    (o1.equal?  nil).should== false
    (nil.equal?  o2).should== false
  end  
  
  it "returns true if obj and anObject have the same value." do 
    o1 = 1
    o2 = :hola
    (:hola.equal? o1).should == false
    (1.equal? o1).should == true
    (:hola.equal? o2).should == true
  end
end
