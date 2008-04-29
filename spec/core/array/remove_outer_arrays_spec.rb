require File.dirname(__FILE__) + '/../../spec_helper'

describe "Array.remove_outer_arrays" do
  it "returns the same Array when no outer Arrays are present" do
    ['a'].send(:remove_outer_arrays).should == ['a']
  end
  
  it "removes outer Array on standard Array instance" do
    [['a']].send(:remove_outer_arrays).should == ['a']
  end
  
  it "removes multiple levels of outer Arrays from Array" do
    [[['a']]].send(:remove_outer_arrays).should == ['a']
  end
  
  it "returns the same Array when no outer Arrays are present on an empty directly recursive Array" do
    a = []
    a << a
    
    # should be updated to use == when it can deal with recursive Arrays
    a.send(:remove_outer_arrays).should.eql?(a)
  end
  
  it "removes outer Arrays on an empty directly recursive Arrays" do
    a = []
    a << a
    
    # should be updated to use == when it can deal with recursive Arrays
    [a].send(:remove_outer_arrays).should.eql?(a)
  end
  
  it "removes multiple levels of outer Arrays from an empty directly recursive Arrays" do
    a = []
    a << a
    
    # should be updated to use == when it can deal with recursive Arrays
    [[a]].send(:remove_outer_arrays).should.eql?(a)
  end
  
  it "returns the same Array when no outer Arrays are present on a populated directly recursive Array" do
    a = ['a']
    a << a
    
    # should be updated to use == when it can deal with recursive Arrays
    a.send(:remove_outer_arrays).should.eql?(a)
  end
  
  it "removes outer Arrays on a populated directly recursive Arrays" do
    a = ['a']
    a << a
    
    # should be updated to use == when it can deal with recursive Arrays
    [a].send(:remove_outer_arrays).should.eql?(a)
  end
  
  it "removes multiple levels of outer Arrays from a populated directly recursive Arrays" do
    a = ['a']
    a << a
    
    # should be updated to use == when it can deal with recursive Arrays
    [[a]].send(:remove_outer_arrays).should.eql?(a)
  end
  
  it "returns the same Array when no outer Arrays are present on an empty indirectly recursive Array" do
    a = []
    b = []
    a << b
    b << a
    
    # should be updated to use == when it can deal with recursive Arrays
    a.send(:remove_outer_arrays).should.eql?(a)
  end
  
  it "removes outers Arrays on an empty indirectly recursive Array" do
    a = []
    b = []
    a << b
    b << a
    
    # should be updated to use == when it can deal with recursive Arrays
    [a].send(:remove_outer_arrays).should.eql?(a)
  end
  
  it "removes multiple levels of outer Arrays from an empty indirectly recursive Arrays" do
    a = []
    b = []
    a << b
    b << a
    
    # should be updated to use == when it can deal with recursive Arrays
    [[a]].send(:remove_outer_arrays).should.eql?(a)
  end
  
  it "returns the same Array when no outer Arrays are present on a populated indirectly recursive Array" do
    a = ['a']
    b = ['b']
    a << b
    b << a
    
    # should be updated to use == when it can deal with recursive Arrays
    a.send(:remove_outer_arrays).should.eql?(a)
  end
  
  it "removes outers Arrays on a populated indirectly recursive Array" do
    a = ['a']
    b = ['b']
    a << b
    b << a
    
    # should be updated to use == when it can deal with recursive Arrays
    [a].send(:remove_outer_arrays).should.eql?(a)
  end
  
  it "removes multiple levels of outer Arrays from a populated indirectly recursive Arrays" do
    a = ['a']
    b = ['b']
    a << b
    b << a
    
    # should be updated to use == when it can deal with recursive Arrays
    [[a]].send(:remove_outer_arrays).should.eql?(a)
  end
end