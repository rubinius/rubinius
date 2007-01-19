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

  specify "unless is the contrary of if, it's happy with false returns" do
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
  specify "simple case" do
    example do
      case 1
        when 1: true
      end
    end.should == true
  end

  specify "for a same result, comparison variables can be collated with a comma" do
    example do
      case 2
        when 1,2: true
      end
    end.should == true
  end

  specify "you can also use ranges for a numeric comparison" do
    example do
      case 5
      when 1..20: true
      end
    end.should == true
  end

  specify "for a default exit, use else" do
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
  
  specify "that if no value is given to case, the default is true" do
    example do
      case
      when 3==3: true
      end
    end.should == true
  end

  # NOTE : This should not work yet, since Onig is not integrated.
  specify "if a string is given, regexp can be used" do
    example do
      case 'hello'
      when /^hell/: true # mouahahaha
      end
    end.should == true
  end

  specify "if class are given as comparators, they are used to compare the var's class (using var.kind_of? i think)" do
    example do
      case 'x'
      when String: true
      end
    end.should == true
  end

end

=begin todo
loop do
  body
end

while bool-expr [do]
 body
end

until bool-expr [do]
 body
end

begin
 body
end while bool-expr

begin
 body
end until bool-expr

for name[, name]... in expr [do]
  body
end

expr.each do | name[, name]... |
  body
end

expr while bool-expr
expr until bool-expr

    * break terminates loop immediately.
    * redo immediately repeats w/o rerunning the condition.
    * next starts the next iteration through the loop.
    * retry restarts the loop, rerunning the condition.

=end
