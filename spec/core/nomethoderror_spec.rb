require File.dirname(__FILE__) + '/../spec_helper'

describe "NoMethodError#args " do 
  before(:each) do 
    class A; end
    class B; end 
  end
  
  it "return a empty array '[ ]'  if the caller method had no arguments" do 
    begin
      B.new.foo
    rescue Exception => e
      e.args.should == [] 
    end
  end
  
  it "return an array with the same elements than the caller method" do 
    begin 
      a = A.new
      B.new.foo(1,a)
    rescue Exception => e
      e.args.should == [1,a]
      e.args[1].object_id.should == a.object_id
    end
  end
end

describe "NoMethodError#message " do
  class A; 
    protected
    def a_protected_method;end
    private 
    def a_private_method; end
  end
  class B; end
  
  it "for an undefined method match /undefined method/" do 
    begin  
      B.new.foo
    rescue Exception => e
      e.class.should == NoMethodError
      # e.message.match( /undefined method/).should_not == nil
    end
  end
  
  it "for an protected method match /protected method/" do
    begin
      A.new.a_private_method
    rescue Exception => e 
      e.class.should == NoMethodError
      # e.message.match(/private method/).should_not == nil
    end
  end
  
  it "for private method match /private method/" do 
    begin
      A.new.a_protected_method
    rescue Exception => e 
      e.class.should == NoMethodError 
      e.message.match(/protected method/).should_not == nil
    end
  end
end