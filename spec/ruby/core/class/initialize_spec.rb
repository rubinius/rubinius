require File.expand_path('../../../spec_helper', __FILE__)

describe "Class#new" do
  it "returns a new instance of self" do
    klass = Class.new
    klass.new.is_a?(klass).should == true
  end

  it "invokes #initialize on the new instance with the given args" do
    klass = Class.new do
      def initialize(*args)
        @initialized = true
        @args = args
      end

      def args
        @args
      end

      def initialized?
        @initialized || false
      end
    end

    klass.new.initialized?.should == true
    klass.new(1, 2, 3).args.should == [1, 2, 3]
  end

  it "passes the block to #initialize" do
    klass = Class.new do
      def initialize
        yield
      end
    end

    klass.new { break 42 }.should == 42
  end
end

describe "Class#initialize" do
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
end
