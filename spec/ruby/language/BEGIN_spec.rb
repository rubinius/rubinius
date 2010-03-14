require File.expand_path('../../spec_helper', __FILE__)

describe "The BEGIN keyword" do
  ruby_version_is "" .. "1.9" do
    it "runs in a new isolated scope" do
      lambda {
        eval "BEGIN { var_in_begin = 'foo' }; var_in_begin"
      }.should raise_error NameError

      lambda {
        outside_var = 'foo'
        eval "BEGIN { outside_var }"
      }.should raise_error NameError
    end
  end

  ruby_version_is "1.9" do
    it "runs in a shared scope" do
      lambda {
        eval "BEGIN { var_in_begin = 'foo' }; var_in_begin"
      }.should_not raise_error NameError

      lambda {
        outside_var = 'foo'
        eval "BEGIN { outside_var }"
      }.should_not raise_error NameError
    end

    it "must appear in a top-level context" do
      lambda do
        eval "1.times{ BEGIN{ :begun }}"
      end.should raise_error(SyntaxError)
    end
  end

  it "runs first in a given code unit" do
    o = Object.new
    class << o
      ARY = []
      eval "ARY << 'foo'; BEGIN { ARY << 'bar' }"
      def ary; ARY; end
    end

    o.ary.should == ['bar', 'foo']
  end

  it "runs multiple begins in FIFO order" do
    o = Object.new
    class << o
      ARY = []
      eval "BEGIN { ARY << 'foo' }; BEGIN { ARY << 'bar' }"
      def ary; ARY; end
    end

    o.ary.should == ['foo', 'bar']
  end
end
