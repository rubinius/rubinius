require File.dirname(__FILE__) + '/../spec_helper'

class AliasObject
  attr :foo
  attr_reader :bar
  attr_accessor :baz
  
  def prep; @foo = 3; @bar = 4; end
  def value; 5; end
  def false_value; 6; end
end

describe "The alias keyword" do
  before(:each) do
    @obj = AliasObject.new
    @meta = class << @obj;self;end
  end

  it "creates a new name for an existing method" do
    @meta.class_eval do
      alias __value value
    end
    @obj.__value.should == 5
  end

  it "adds the new method to the list of methods" do
    original_methods = @obj.methods
    @meta.class_eval do
      alias __value value
    end
    (@obj.methods - original_methods).should == ["__value"]
  end

  it "adds the new method to the list of public methods" do
    original_methods = @obj.public_methods
    @meta.class_eval do
      alias __value value
    end
    (@obj.public_methods - original_methods).should == ["__value"]
  end

  it "overwrites an existing method with the target name" do
    @meta.class_eval do
      alias false_value value
    end
    @obj.false_value.should == 5
  end

  it "is reversible" do
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

  it "operates on the object's metaclass when used in instance_eval" do
    @obj.instance_eval do
      alias __value value
    end

    @obj.__value.should == 5
    lambda { AliasObject.new.__value }.should raise_error(NoMethodError)
  end

  it "operates on methods defined via attr, attr_reader, and attr_accessor" do
    @obj.prep
    @obj.instance_eval do
      alias afoo foo
      alias abar bar
      alias abaz baz
    end

    @obj.afoo.should == 3
    @obj.abar.should == 4
    @obj.baz = 5
    @obj.abaz.should == 5
  end
end
