require File.dirname(__FILE__) + '/../spec_helper'

context "Kernel.Float()" do
  specify "should call to_f to convert any arbitrary argument to a Float" do
    example do
      class Foo
        def to_f; 1.1; end
      end

      Float( Foo.new )
    end.should == 1.1
  end
  
  specify "should call to_i to convert any arbitrary argument to a Float" do
    example do
      class Foo
        def to_i; 7; end
      end

      Float( Foo.new )
    end.should == 7.0
  end

  specify "should give to_f precedence over to_i" do
    example do
      class Foo
        def to_i; 7; end
        def to_f; 69.9; end
      end

      Float( Foo.new )
    end.should == 69.9
  end

  specify "should raise a TypeError if there is no to_f or to_i method on an object" do
    example do
      class Foo; end

      Float( Foo.new)
    end.should_raise(TypeError)
  end

  specify "should raise a TypeError if to_f doesn't return a Float" do
    example do
      class Foo
        def to_f; 'har'; end
      end

      Float( Foo.new)
    end.should_raise(TypeError)
  end
end


context "Kernel.Integer()" do
  specify "should call to_i to convert any arbitrary argument to an Integer" do
    example do
      class Foo
        def to_i; 7; end
      end

      Integer( Foo.new )
    end.should == 7
  end

  specify "should raise a TypeError if there is no to_i method on an object" do
    example do
      class Foo; end

      Integer( Foo.new )
    end.should_raise(TypeError)
  end

  specify "should raise a TypeError if to_i doesn't return an Integer" do
    example do
      class Foo
        def to_i; 'har'; end
      end

      Integer( Foo.new )
    end.should_raise(TypeError)
  end
end

