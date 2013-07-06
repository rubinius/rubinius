require File.expand_path('../../../spec_helper', __FILE__)

describe "Class#initialize" do
  it "is private" do
    Class.should have_private_method(:initialize)
  end

  it "raises a TypeError when called on already initialized classes" do
    lambda{
      Fixnum.send :initialize
    }.should raise_error(TypeError)

    lambda{
      Object.send :initialize
    }.should raise_error(TypeError)
  end

  ruby_version_is "1.9" do
    # See [redmine:2601]
    it "raises a TypeError when called on BasicObject" do
      lambda{
        BasicObject.send :initialize
      }.should raise_error(TypeError)
    end
  end

  describe "when given the Class" do
    before(:each) do
      @uninitialized = Class.allocate
    end

    ruby_version_is ""..."1.9" do
      it "becomes a subclass of the Class" do
        @uninitialized.send :initialize, Class
        @uninitialized.superclass.should equal(Class)
      end
    end

    ruby_version_is "1.9" do
      it "raises a TypeError" do
        lambda{@uninitialized.send(:initialize, Class)}.should raise_error(TypeError)
      end
    end
  end
end
