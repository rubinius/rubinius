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

context "A Proc instance created using Proc.given" do
  specify "should be nil if no block given" do
    example do
      def answer
        Proc.given.nil?
      end
      answer
    end.should == true
  end

  specify "should reference the same block" do
    example do
      def answer(&prc)
        prc.block.object_id == Proc.given.block.object_id
      end
      answer { }
    end.should == true
  end

  specify "should be able to call the block" do
    example do
      def answer(&prc)
        a = [prc.nil?, Proc.given.nil?]
         a << (block_given? == !Proc.given.nil?)
         if block_given?
            a << (prc.object_id == Proc.given.object_id)
            a << (prc.block.object_id == Proc.given.block.object_id)
            a << Proc.given.call(21)
         end
        a
      end
      
      [answer, answer { |n| n * 2} ]
    end.should == [ [true, true, true], [false, false, true, false, true, 42]]
  end

  specify "using a MethodContext should do the mind trick" do
    example do
      def stormtrooper
        yield "Let me see your identification."
        obiwan { |reply| puts "Obi-Wan: #{reply}" }
      end
      
      def obiwan
        yield "[with a small wave of his hand] You don't need to see his identification."
        ctx = MethodContext.current.sender
        Proc.given(ctx).call("We don't need to see his identification.")
      end
      
      stormtrooper { |msg| puts "Stormtrooper: #{msg}" }
    end.stdout.should == <<-OUT.unindent
      Stormtrooper: Let me see your identification.
      Obi-Wan: [with a small wave of his hand] You don't need to see his identification.
      Stormtrooper: We don't need to see his identification.
    OUT
  end

end
