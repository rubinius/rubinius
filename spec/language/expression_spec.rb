require File.dirname(__FILE__) + '/../spec_helper'

context "if expression" do
  specify "should evaluate body when the if expression is true" do
    example do
      @a = if true: true; end
      @b = if true then true; end
      @c = if true
        true
      end
      [@a, @b, @c]
    end.should == [true, true, true]
  end
  
  specify "should not evaluate body when the if expression is false" do
    example do
      @a = if false: true; end
      @b = if false then true; end
      @c = if false
        true
      end
      [@a, @b, @c]
    end.should == [nil, nil, nil]
  end
  
  specify "should not evaluate the else body when the if expression is true" do
    example do
      @a = if true: true; else false; end
      @b = if true then true; else false; end
      @c = if true: true
      else
        false
      end
      @d = if true
        true
      else
        false
      end
      [@a, @b, @c, @d]
    end.should == [true, true, true, true]
  end
  
  specify "should evaluate the else body when the if expression is false" do
    example do
      @a = if false: true; else false; end
      @b = if false then true; else false; end
      @c = if false: true
      else
        false
      end
      @d = if false
        true
      else
        false
      end
      [@a, @b, @c, @d]
    end.should == [false, false, false, false]
  end
end
