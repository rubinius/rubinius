require File.dirname(__FILE__) + '/../spec_helper'

class AliasObject
  def value; 5; end
  def false_value; 6; end
end

describe "The alias keyword" do
  before(:each) do
    @obj = AliasObject.new
    @meta = class << @obj;self;end
  end

  it "should create a new name for an existing method" do
    @meta.class_eval do
      alias __value value
    end
    @obj.__value.should == 5
  end

  it "should overwrite an existing method with the target name" do
    @meta.class_eval do
      alias false_value value
    end
    @obj.false_value.should == 5
  end

  it "should be reversible" do
    @meta.class_eval do
      alias __value value
      alias value false_value
    end
    @obj.value.should == 6

    @meta.class_eval do
      alias value __value
    end
    @obj.value.should == 5
  end
end
