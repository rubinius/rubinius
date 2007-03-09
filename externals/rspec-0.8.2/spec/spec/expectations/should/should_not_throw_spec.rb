require File.dirname(__FILE__) + '/../../../spec_helper.rb'

context "should_not_throw" do
  specify "should fail when expected symbol is actually thrown" do
    lambda do
      lambda do
        throw(:foo)
      end.should_not_throw(:foo)
    end.should_fail
  end

  specify "should pass when expected symbol is thrown" do
    lambda do
      lambda do
        throw(:bar)
      end.should_not_throw(:foo)
    end.should_not_raise
  end

  specify "should pass when no symbol is thrown" do
    lambda do
      lambda do
        "".to_s
      end.should_not_throw(:foo)
    end.should_not_raise
  end

  specify "should pass when no symbol is thrown and none is specified" do
    lambda do
      lambda do
        "".to_s
      end.should_not_throw
    end.should_not_raise
  end
end
