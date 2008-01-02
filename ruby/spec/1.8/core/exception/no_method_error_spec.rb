require File.dirname(__FILE__) + '/../../spec_helper'

describe "NoMethodError.new" do
  it "allows passing method args" do
    NoMethodError.new("msg","name","args").args.should == "args"
  end
end

class NoMethodErrorA; end
class NoMethodErrorB; end

describe "NoMethodError#args" do 
  it "returns an empty array if the caller method had no arguments" do 
    begin
      NoMethodErrorB.new.foo
    rescue Exception => e
      e.args.should == [] 
    end
  end
  
  it "returns an array with the same elements as passed to the method" do 
    begin 
      a = NoMethodErrorA.new
      NoMethodErrorB.new.foo(1,a)
    rescue Exception => e
      e.args.should == [1,a]
      e.args[1].object_id.should == a.object_id
    end
  end
end

class NoMethodErrorC; 
  protected
  def a_protected_method;end
  private 
  def a_private_method; end
end
class NoMethodErrorD; end

describe "NoMethodError#message" do  
  it "for an undefined method match /undefined method/" do 
    begin  
      NoMethodErrorD.new.foo
    rescue Exception => e
      e.class.should == NoMethodError
    end
  end
  
  it "for an protected method match /protected method/" do
    begin
      NoMethodErrorC.new.a_private_method
    rescue Exception => e 
      e.class.should == NoMethodError
    end
  end
  
  not_compliant_on :rubinius do
    it "for private method match /private method/" do 
      begin
        NoMethodErrorC.new.a_protected_method
      rescue Exception => e 
        e.class.should == NoMethodError 
        e.message.match(/private method/).should_not == nil
      end
    end
  end
end
