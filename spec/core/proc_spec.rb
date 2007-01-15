require File.dirname(__FILE__) + '/../spec_helper'

context "A Proc instance" do
  setup do 
    @src = code do
      @prc = lambda { "Software is the source, not the binaries" }
    end
  end

  specify "created using &nil should be nil" do
    example do
      def a(&prc)
        prc.object_id
      end
      nil.object_id == a(&nil)
    end.should == true
  end
  
  specify "should be created from an object responding to :to_proc" do
    example do
      class Symbol
        def to_proc
          lambda { self }
        end
      end
      def a(&prc)
        prc.call
      end
      a(&:to_s)
    end.should == :to_s
  end
  
  specify "should respond to :call" do
    example(@src) do
      @prc.respond_to?(:call)
    end.should == true
  end

  specify "should respond to :to_proc" do
    example(@src) do 
      @prc.respond_to? :to_proc
    end.should == true
  end

  specify "to_proc should return the same instance" do
    example(@src) do
      @prc.object_id == @prc.to_proc.object_id
    end.should == true
  end
  
  specify "should be the same if passed across methods" do
    example do
      def a(&prc)
        b(&prc)
      end
      
      def b(&prc)
        c(&prc)
      end
      
      def c(&prc)
        prc.object_id
      end

      @p.object_id == a(&@p)
    end.should == true
  end
end

