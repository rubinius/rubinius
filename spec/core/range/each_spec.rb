require File.dirname(__FILE__) + '/../../spec_helper'

describe "Range#each" do
  it "passes each element to the given block by using #succ" do
    a = []
    (-5..5).each { |i| a << i }
    a.should == [-5, -4, -3, -2, -1, 0, 1, 2, 3, 4, 5]

    a = []
    ('A'..'D').each { |i| a << i }
    a.should == ['A','B','C','D']

    a = []
    ('A'...'D').each { |i| a << i }
    a.should == ['A','B','C']
    
    a = []
    (0xfffd...0xffff).each { |i| a << i }
    a.should == [0xfffd,0xfffe]

    y = Object.new
    x = Object.new
    x.should_receive(:method_missing, :with => [:<=>, y], :count => :any, :returning => -1)
    x.should_receive(:succ, :returning => y)
    y.should_receive(:method_missing, :with => [:<=>, x], :count => :any, :returning => 0)
    
    a = []
    (x..y).each { |i| a << i }
    a.should == [x]
  end
  
  it "raises a TypeError if the first element does not respond to #succ" do
    should_raise(TypeError, "can't iterate from Float") do
      (0.5..2.4).each { |i| i }
    end
    
    b = Object.new
    (a = Object.new).should_receive(:method_missing, :with => [:<=>, b], :returning => 1)
    
    should_raise(TypeError, "can't iterate from Object") do
      (a..b).each { |i| i }
    end
  end
  
  it "returns self" do
    (1..10).each {}.should == (1..10)
  end
end
