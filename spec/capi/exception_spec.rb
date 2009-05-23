require File.dirname(__FILE__) + '/spec_helper'

load_extension("exception")

describe "C-API Exception function" do
  before :each do
    @s = CApiExceptionSpecs.new
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

  describe "rb_exc_raise" do
    it "raises passed exception" do
      runtime_error = RuntimeError.new '42'
      lambda { @s.rb_exc_raise(runtime_error) }.should raise_error(RuntimeError, '42')
    end
  end
end
