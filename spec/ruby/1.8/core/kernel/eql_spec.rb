require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Kernel#eql?" do 
  it "returns true if obj and anObject are the same object." do 
    o1 = mock('o1')
    o2 = mock('o2')
    (o1.eql?(o1)).should == true
    (o2.eql?(o2)).should == true
    (o1.eql?(o2)).should == false
  end
  
  it "returns true if obj and anObject have the same value." do 
    o1 = 1
    o2 = :hola
    (:hola.eql? o1).should == false
    (1.eql? o1).should == true
    (:hola.eql? o2).should == true
  end
end

