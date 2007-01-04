require File.dirname(__FILE__) + '/../spec_helper'

# <, <=, ==, >, >=, between?

context "A class with Comparable mixin, method" do
  setup do
    @src = <<-CODE
    class Weird
      include Comparable
      
      def initialize(int)
        @int = int
      end
      
      def negative?
        @int < 0
      end
      
      def <=>(other)
        return 0 if self.negative? == other.negative?
        return 1 if self.negative?
        -1
      end
    end
    CODE
  end
  
  specify "<=> should be provided" do
    example(@src) do
      p Weird.new(0).respond_to?(:<=>)
    end.should == 'true'
  end
  
  specify "<=> should return 0 if other is equal" do
    example(@src) do
      puts Weird.new(-1) <=> Weird.new(-2)
    end.should == '0'
  end
  
  specify "<=> should return 1 if other is greater" do
    example(@src) do
      puts Weird.new(-1) <=> Weird.new(0)
    end.should == '1'
  end
  
  specify "<=> should return -1 if other is lesser" do
    example(@src) do
      puts Weird.new(1) <=> Weird.new(-1)
    end.should == '-1'
  end
  
  specify "< should return true if other is greater" do
    example(@src) do
      puts Weird.new(1) < Weird.new(-1)
    end.should == 'true'
  end
  
  specify "< should return false if other is lesser than or equal" do
    example(@src) do
      @a = Weird.new(-1)
      @b = Weird.new(0)
      p [@a < @b, @a < @a, @b < @b]
    end.should == '[false, false, false]'
  end
  
  specify "<= should return true if other is greater than or equal" do
    example(@src) do
      @a = Weird.new(0)
      @b = Weird.new(-1)
      p [@a <= @b, @a <= @a, @b <= @b]
    end.should == '[true, true, true]'
  end
  
  specify "<= should return false if other is lesser" do
    example(@src) do
      puts Weird.new(-1) <= Weird.new(0)
    end.should == 'false'
  end

  specify "== should return true if other is equal" do
    example(@src) do
      @a = Weird.new(0)
      @b = Weird.new(-1)
      @c = Weird.new(1)
      p [@a == @c, @a == @a, @b == @b, @c == @c]
    end.should == '[true, true, true, true]'
  end

  specify "== should return false if other is not equal" do
    example(@src) do
      @a = Weird.new(0)
      @b = Weird.new(-1)
      @c = Weird.new(1)
      p [@a == @b, @b == @c]
    end.should == '[false, false]'
  end

  specify "> should return true if other is lesser" do
    example(@src) do
      puts Weird.new(-1) > Weird.new(0)
    end.should == 'true'
  end

  specify "> should return false if other is greater than or equal" do
    example(@src) do
      @a = Weird.new(-1)
      @b = Weird.new(0)
      p [@b > @a, @a > @a, @b > @b]
    end.should == '[false, false, false]'
  end

  specify ">= should return true if other is lesser than or equal" do
    example(@src) do
      @a = Weird.new(-1)
      @b = Weird.new(0)
      @c = Weird.new(1)
      p [@b <= @a, @b <= @c]
    end.should == '[true, true]'
  end

  specify ">= should return false if other is greater" do
    example(@src) do
      p Weird.new(1) >= Weird.new(-1)
    end.should == 'false'
  end

  specify "betweem? should return true if min <= self <= max" do
    example(@src) do
      @a = Weird.new(-1)
      @b = Weird.new(0)
      @c = Weird.new(1)
      p [@b.between?(@c, @a), @c.between?(@b, @a)]
    end.should == '[true, true]'
  end
  
  specify "between? should return false if self < min or self > max" do
    example(@src) do
      @a = Weird.new(-1)
      @b = Weird.new(0)
      @c = Weird.new(1)
      p [@a.between?(@b, @c), @a.between?(@c, @b)]
    end.should == '[false, false]'
  end
end
