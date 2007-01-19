require File.dirname(__FILE__) + '/../../spec_helper'

context "A ruby environment" do
  setup do
    @src = code do
      class Foo
        def no_args
        end
        def args(x)
        end
      end
      
      class Bar < Foo
        def no_args
          defined?(super)
        end
        def args
          defined?( super() )
        end
      end
    end
  end
  
  specify "should return true when defined?(puts) is sent" do
    example do
      !!defined?(puts)
    end.should == true
  end

  specify "should return true when defined?(Kernel.puts) is sent (attribute)" do
    example do
      !!defined?(Kernel.puts)
    end.should == true
  end

  specify "should return false when defined?(DoesNotExist.puts) is sent" do
    example do
      !!defined?(DoesNotExist.puts)
    end.should == false
  end

  specify "should return true when defined?(x = 2) is sent" do
    example do
      !!defined?(x = 2)
    end.should == true
  end

  specify "should return true when defined?(Object) is sent" do
    example do
      !!defined?(Object)
    end.should == true
  end

  specify "should return true when @@x = 1; defined?(@@x) is sent" do
    example do
      @@x = 1
      !!defined?(@@x)
    end.should == true
  end

  specify "should return true when @x = 1; defined?(@x) is sent" do
    example do
      @x = 1
      !!defined?(@x)
    end.should == true
  end

  specify "should return true when $x = 1; defined?($x) is sent" do
    example do
      $x = 1
      !!defined?($x)
    end.should == true
  end

  specify "should return true when x = 1; defined?(x) is sent" do
    example do
      x = 1
      !!defined?(x)
    end.should == true
  end

  specify "should return true when defined? is called on a block var" do
    example do
      block = Proc.new {|x| defined?(x) }
      !!block.call(1)
    end.should == true
  end

  specify "should return true when defined?('foo = bar') is sent" do
    example do
      !!defined?('foo = bar')
    end.should == true
  end

  specify "should return true when defined?(self) is sent" do
    example do
      !!defined?(self)
    end.should == true
  end

  specify "should return true when defined?(nil) is sent" do
    example do
      !!defined?(nil)
    end.should == true
  end

  specify "should return true when defined?(true) is sent" do
    example do
      defined?(true)
    end.should == "true"
  end

  specify "should return true when defined?(false) is sent" do
    example do
      defined?(false)
    end.should == "false"
  end

  specify "should return false when defined?(x) is sent" do
    example do
      !!defined?(x)
    end.should == false
  end

  specify "should return true when Bar#no_args uses defined?" do
    example(@src) do
      b = Bar.new
      !!b.no_args
    end.should == true
  end

  specify "should return true when Bar#args uses defined?" do
    example(@src) do
      b = Bar.new
      !!b.args
    end.should == true
  end
end

