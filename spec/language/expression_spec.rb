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

context "unless expression" do
  specify "unless should evaluate the else body when the unless expression is false" do
    example do
      @a = unless false: true; else false; end
      @b = unless false then true; else false; end
      @c = unless false: true
      else 
        false
      end
      @d = unless false
        true
      else
        false
      end
      [@a, @b, @c, @d]
    end.should == [true, true, true, true]
  end
  
end
context "case expression" do
  specify "should evaluate the body of the when clause whose expression matches the case target expression" do
    example do
      case 1
        when 1: true
      end
    end.should == true
  end

  specify "should evaluate the body of the when clause whose array expression includes the case target expression" do
    example do
      case 2
        when 1,2: true
      end
    end.should == true
  end

  specify "should evaluate the body of the when clause whose range expression includes the case target expression" do
    example do
      case 5
      when 1..20: true
      end
    end.should == true
  end

  specify "should evaluate the body of the else clause if no when expressions match the case target expression" do
    example do
      case 3
      when 6: false
      else
        true
      end
    end.should == true
  end

=begin FIXME: The Exception is not handled well
  specify "but you can't use else without the when construct" do
    example do
      case 4
      else
        true
      end
    end.should_raise SyntaxError # (unexpected kELSE, expecting kWHEN)
  end
=end
  
  specify "should evaluate the body of the first when clause that is true when no case target expression is given" do
    example do
      case
      when 3==3: true
      when 4==4: false
      end
    end.should == true
  end

  # NOTE : This should not work yet, since Onig is not integrated.
  specify "should evaluate the body of the when clause whose expression is a regex that matches the case target expression" do
    example do
      case 'hello'
      when /^hell/: true # mouahahaha
      end
    end.should == true
  end

  specify "should evaluate the body of the when clause whose expression is a class using class === case target expression" do
    example do
      case 'x'
      when String: true
      end
    end.should == true
  end

end

# * break terminates loop immediately.
# * redo immediately repeats w/o rerunning the condition.
# * next starts the next iteration through the loop.
# * retry restarts the loop, rerunning the condition.

context "loop" do
  # loop do
  #   body
  # end
end

context "while expression" do
  
  # while bool-expr [do]
  #  body
  # end

  # begin
  #  body
  # end until bool-expr

  # expr while bool-expr

end

context "until expression" do
  # until bool-expr [do]
  #  body
  # end

  # begin
  #  body
  # end while bool-expr

  # expr until bool-expr

end

context "for expression" do
  # for name[, name]... in expr [do]
  #   body
  # end
end
