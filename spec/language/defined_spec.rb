require File.dirname(__FILE__) + '/../spec_helper'

context "A ruby environment" do
  specify "should return true when defined?(puts) is sent" do
    example do
      puts !!defined?(puts)
    end.should == 'true'
  end

  specify "should return true when defined?(Kernel.puts) is sent (attribute)" do
    example do
      puts !!defined?(Kernel.puts)
    end.should == 'true'
  end

  specify "should return true when defined?(x = 2) is sent" do
    example do
      puts !!defined?(x = 2)
    end.should == 'true'
  end

  specify "should return true when defined?(Object) is sent" do
    example do
      puts !!defined?(Object)
    end.should == 'true'
  end

  specify "should return true when @@x = 1; defined?(@@x) is sent" do
    example do
      @@x = 1
      puts !!defined?(@@x)
    end.should == 'true'
  end

  specify "should return true when @x = 1; defined?(@x) is sent" do
    example do
      @x = 1
      puts !!defined?(@x)
    end.should == 'true'
  end

  specify "should return true when $x = 1; defined?($x) is sent" do
    example do
      $x = 1
      puts !!defined?($x)
    end.should == 'true'
  end

  specify "should return true when x = 1; defined?(x) is sent" do
    example do
      x = 1
      puts !!defined?(x)
    end.should == 'true'
  end

  specify "should return true when defined? is called on a block var" do
    example do
      block = Proc.new {|x| defined?(x) }
      puts !!block.call(1)
    end.should == 'true'
  end

  specify "should return true when defined?('foo = bar') is sent" do
    example do
      puts !!defined?('foo = bar')
    end.should == 'true'
  end

  specify "should return true when defined?(self) is sent" do
    example do
      puts !!defined?(self)
    end.should == 'true'
  end

  specify "should return true when defined?(nil) is sent" do
    example do
      puts !!defined?(nil)
    end.should == 'true'
  end

  specify "should return true when defined?(true) is sent" do
    example do
      puts defined?(true)
    end.should == 'true'
  end

  specify "should return true when defined?(nil) is sent" do
    example do
      puts defined?(false)
    end.should == 'false'
  end

  specify "should return false when defined?(x) is sent" do
    example do
      puts !!defined?(x)
    end.should == 'false'
  end

  specify "should return true when Bar#no_args uses defined?" do
    example do
      require File.dirname(__FILE__) + '/../defined_fixtures'
      b = Bar.new
      puts !!b.no_args
    end.should == 'true'
  end

  specify "should return true when Bar#args uses defined?" do
    example do
      require File.dirname(__FILE__) + '/../defined_fixtures'
      b = Bar.new
      puts !!b.args
    end.should == 'true'
  end

end

