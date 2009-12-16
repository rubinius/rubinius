require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + "/fixtures/classes"
require 'delegate'

describe "SimpleDelegator.new" do
  before :each do
    @simple = DelegateSpecs::Simple.new
    @delegate = SimpleDelegator.new(@simple)
  end

  it "forwards public method calls" do
    @delegate.pub.should == :foo
  end

  it "doesn't forward private method calls" do
    lambda{ @delegate.priv }.should raise_error( NoMethodError )
  end

  ruby_bug "redmine:2206", "1.8.7" do
    it "forwards private method calls made via send or __send__" do
      @delegate.send(:priv, 42).should == [:priv, 42]
      @delegate.__send__(:priv, 42).should == [:priv, 42]
      lambda{ @delegate.__send__(:priv, 42){@delegate.priv} }.should raise_error( NoMethodError )
    end
  end
end
