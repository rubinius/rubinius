require File.dirname(__FILE__) + '/../spec_helper'
class SpecificExampleException < StandardError
end
class OtherCustomException < StandardError
end

def exception_list
  [SpecificExampleException, ZeroDivisionError]
end
describe "The rescue keyword" do
  it "can be used to handle a specific exception" do
    lambda do
      begin
        raise SpecificExampleException, "Raising this to be handled below"
      rescue SpecificExampleException
      end
    end.should_not raise_error
  end
  
  it "can capture the raised exception in a local variable" do
    begin
      raise SpecificExampleException, "some text"
    rescue SpecificExampleException => e
      e.message.should == "some text"
    end
  end
  
  it "can rescue multiple raised exceptions with a single rescue block" do
    lambda do
      [lambda{1/0}, lambda{raise SpecificExampleException}].each do |block|
        begin
          block.call
        rescue SpecificExampleException, ZeroDivisionError
        end
      end
    end.should_not raise_error
  end
  
  it "can rescue a splatted list of exceptions" do
    caught_it = false
    begin
      raise SpecificExampleException, "not important"
    rescue *exception_list
      caught_it = true
    end
    caught_it.should be_true
    caught = []
    lambda do
      [lambda{1/0}, lambda{raise SpecificExampleException}].each do |block|
        begin
          block.call
        rescue *exception_list
          caught << $!
        end
      end
    end.should_not raise_error    
    caught.size.should == 2
    exception_list.each do |exception_class|
      caught.map{|e| e.class}.include?(exception_class).should be_true
    end
  end
  
  it "will only rescue the specified exceptions when doing a splat rescue" do
    lambda do
      begin
        raise OtherCustomException, "not rescued!"
      rescue *exception_list
      end
    end.should raise_error(OtherCustomException)
  end
  
end