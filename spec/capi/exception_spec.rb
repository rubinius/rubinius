require File.dirname(__FILE__) + '/spec_helper'

load_extension("exception")

describe "C-API Exception function" do
  before :each do
    @s = CApiExceptionSpecs.new
  end

  describe "rb_raise" do
    it "raises an exception" do
      lambda { @s.rb_raise({}) }.should raise_error(TypeError)
    end

    it "terminates the function at the point it was called" do
      h = {}
      lambda { @s.rb_raise(h) }.should raise_error(TypeError)
      h[:stage].should == :before
    end
  end

  describe "rb_exc_new" do
    it "creates an exception from a C string and length" do
      @s.rb_exc_new('foo').to_s.should == 'foo'
    end
  end

  describe "rb_exc_new2" do
    it "creates an exception from a C string" do
      @s.rb_exc_new2('foo').to_s.should == 'foo'
    end
  end

  describe "rb_exc_new3" do
    it "creates an exception from a Ruby string" do
      @s.rb_exc_new3('foo').to_s.should == 'foo'
    end
  end
end
