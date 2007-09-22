require File.dirname(__FILE__) + '/../../spec_helper'

describe "Range.new" do
  it "constructs a range using the given start and end" do
    range = Range.new('a', 'c')
    range.should == ('a'..'c')
    
    range.first.should == 'a'
    range.last.should == 'c'
  end
  
  it "includes the end object when the third parameter is omitted or false" do
    Range.new('a', 'c').to_a.should == ['a', 'b', 'c']
    Range.new(1, 3).to_a.should == [1, 2, 3]
    
    Range.new('a', 'c', false).to_a.should == ['a', 'b', 'c']
    Range.new(1, 3, false).to_a.should == [1, 2, 3]
    
    Range.new('a', 'c', true).to_a.should == ['a', 'b']
    Range.new(1, 3, 1).to_a.should == [1, 2]
    
    Range.new(1, 3, Object.new).to_a.should == [1, 2]
    Range.new(1, 3, :test).to_a.should == [1, 2]
  end
  
  it "raises an ArgumentError when the given start and end can't be compared by using #<=>" do
    should_raise(ArgumentError, "bad value for range") do
      Range.new(1, '3')
    end
    
    should_raise(ArgumentError, "bad value for range") do
      Range.new(Object.new, Object.new)
    end
    
    b = Object.new
    (a = Object.new).should_receive(:method_missing, :with => [:<=>, b], :returning => nil)
    should_raise(ArgumentError, "bad value for range") do
      Range.new(a, b)
    end
  end
end

describe "Range#initialize" do
  it "can't be called twice" do
    [0..1, 'a'..'b'].each do |range|
      should_raise(NameError, "`initialize' called twice") do
        range.send(:initialize, range.begin, range.end)
      end
    end
  end
end