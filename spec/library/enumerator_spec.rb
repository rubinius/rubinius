require File.dirname(__FILE__) + '/../spec_helper'

# each_cons, each_slice, enum_cons, enum_slice, enum_with_index  

context "A class with Enumerable::Enumerator mixin" do
  setup do
    @src = <<-CODE
    require 'enumerator'
    
    class Numerous
      include Enumerable
      
      def initialize(*list)
        @list = list.empty? ? [2, 5, 3, 6, 1, 4] : list
      end
      
      def each
        @list.each { |i| yield i }
      end
      
    end
    CODE
  end

  specify "each_cons should iterate the block for each array of n consecutive elements" do
    example(@src) do
      Numerous.new.each_cons(4) { |a| p a }
    end.should == "[2, 5, 3, 6]\n[5, 3, 6, 1]\n[3, 6, 1, 4]"
  end
  
  specify "each_slice should " do
    example(@src) do
    end.should == ''
  end
  
  specify "enum_cons should " do
    example(@src) do
    end.should == ''
  end
  
  specify "enum_slice should " do
    example(@src) do
    end.should == ''
  end
  
  specify "enum_with_index should " do
    example(@src) do
    end.should == ''
  end
end
