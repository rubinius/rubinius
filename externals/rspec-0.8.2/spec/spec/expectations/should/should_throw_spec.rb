require File.dirname(__FILE__) + '/../../../spec_helper.rb'

context "should_throw" do

  specify "should fail when no symbol is thrown" do
    lambda do
      lambda {}.should_throw(:foo)
    end.should_fail
  end

  specify "should fail when wrong symbol is thrown" do
    lambda do
      lambda do
        throw(:bar)
      end.should_throw(:foo)
    end.should_fail
  end

  specify "should pass when proper symbol is thrown" do
    lambda do
      lambda do
        throw(:foo)
      end.should_throw(:foo)
    end.should_not_raise
  end

end
