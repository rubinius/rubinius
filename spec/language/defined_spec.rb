require File.dirname(__FILE__) + '/../spec_helper'

context "A ruby environment" do
  specify "should return 'method' when defined?(puts) is sent" do
    example do
      puts defined?(puts)
    end.should == 'method'
  end

  specify "should return 'method' when defined?(Kernel.puts) is sent (attribute)" do
    example do
      puts defined?(Kernel.puts)
    end.should == 'method'
  end

  specify "should return 'assignment' when defined?(x = 2) is sent" do
    example do
      puts defined?(x = 2)
    end.should == 'assignment'
  end

  specify "should return 'constant' when defined?(Object) is sent" do
    example do
      puts defined?(Object)
    end.should == 'constant'
  end

  specify "should return 'class variable' when @@x = 1; defined?(@@x) is sent" do
    example do
      @@x = 1
      puts defined?(@@x)
    end.should == 'class variable'
  end

  specify "should return 'class variable' when @x = 1; defined?(@x) is sent" do
    example do
      @x = 1
      puts defined?(@x)
    end.should == 'instance-variable'
  end

  specify "should return 'global-variable' when $x = 1; defined?($x) is sent" do
    example do
      $x = 1
      puts defined?($x)
    end.should == 'global-variable'
  end

  specify "should return 'local-variable' when x = 1; defined?(x) is sent" do
    example do
      x = 1
      puts defined?(x)
    end.should == 'local-variable'
  end

  specify "should return 'local-variable(in-block)' when defined? is called on a block var" do
    example do
      block = Proc.new {|x| defined?(x) }
      puts block.call(1)
    end.should == 'local-variable(in-block)'
  end

  specify "should return 'expression' when defined?('foo = bar') is sent" do
    example do
      puts defined?('foo = bar')
    end.should == 'expression'
  end

  specify "should return 'self' when defined?(self) is sent" do
    example do
      puts defined?(self)
    end.should == 'self'
  end

  specify "should return 'nil' when defined?(nil) is sent" do
    example do
      puts defined?(nil)
    end.should == 'nil'
  end

  specify "should return 'true' when defined?(true) is sent" do
    example do
      puts defined?(true)
    end.should == 'true'
  end

  specify "should return 'false' when defined?(nil) is sent" do
    example do
      puts defined?(false)
    end.should == 'false'
  end

  specify "should return 'super' when defined?(Bar#no_args) is sent" do
    example do
      require File.dirname(__FILE__) + '/../defined_fixtures'
      b = Bar.new
      puts b.no_args
    end.should == 'super'
  end

  specify "should return 'zsuper' when defined?(Bar#no_args) is sent" do
    example do
      require File.dirname(__FILE__) + '/../defined_fixtures'
      b = Bar.new
      puts b.args
    end.should == 'zsuper'
  end
end

