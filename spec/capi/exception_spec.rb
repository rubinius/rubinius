require File.dirname(__FILE__) + '/spec_helper'

load_extension("exception")

describe "CApiExceptionSpecs" do
  before :each do
    @s = SubtendRaiser.new
  end

  it "rb_raise should raise an exception" do
    lambda { @s.raise! }.should raise_error(TypeError)
  end

  it "rb_raise terminates the function early" do
    h = {}
    lambda { @s.raise_early(h) }.should raise_error(TypeError)
    h[:screwed].should == false
  end

  it "rb_exc_new should instantiate an exception with a string argument and return it" do
    @s.rb_exc_new('foo').to_s.should == 'foo'
  end

  it "rb_exc_new2 should instantiate an exception with a string argument and return it" do
    @s.rb_exc_new2('foo').to_s.should == 'foo'
  end

  it "rb_exc_new3 should instantiate an exception with a string argument and return it" do
    @s.rb_exc_new3('foo').to_s.should == 'foo'
  end
end
