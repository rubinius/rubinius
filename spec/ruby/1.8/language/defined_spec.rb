require File.dirname(__FILE__) + '/../spec_helper'

describe "A ruby environment" do
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
  
  it "returns true when defined?(exit) is sent" do
    !!defined?(exit).should == true
  end

  it "returns true when defined?(Kernel.puts) is sent (attribute)" do
    !!defined?(Kernel.puts).should == true
  end

  it "returns false when defined?(DoesNotExist.puts) is sent" do
    !!defined?(DoesNotExist.puts).should == false
  end

  it "returns true when defined?(x = 2) is sent" do
    !!defined?(x = 2).should == true
  end

  it "returns true when defined?(Object) is sent" do
    !!defined?(Object).should == true
  end

  it "returns true when @@x = 1; defined?(@@x) is sent" do
    @@x = 1
    !!defined?(@@x).should == true
  end

  it "returns true when @x = 1; defined?(@x) is sent" do
    @x = 1
    !!defined?(@x).should == true
  end

  it "returns true when $x = 1; defined?($x) is sent" do
    $x = 1
    !!defined?($x).should == true
  end

  it "returns true when x = 1; defined?(x) is sent" do
    x = 1
    !!defined?(x).should == true
  end

  it "returns true when defined? is called on a block var" do
    block = Proc.new { |x| defined?(x) }
    !!(block.call(1)).should == true
  end

  it "returns true when defined?('foo = bar') is sent" do
    !!defined?('foo = bar').should == true
  end

  it "returns true when defined?(self) is sent" do
    !!defined?(self).should == true
  end

  it "returns true when defined?(nil) is sent" do
    !!defined?(nil).should == true
  end

  it "returns true when defined?(true) is sent" do
    !!defined?(true).should == true
  end

  it "returns true when defined?(false) is sent" do
    !!defined?(false).should == true
  end

  it "returns false when defined?(x) is sent" do
    !!defined?(x).should == false
  end

  it "returns true when defined?(:File) is sent" do
    !!defined?(:File).should == true
  end

  it "returns 'constant' when defined?(File) is sent" do
    defined?(File).should == "constant"
  end

  it "returns 'constant' when defined?(File::SEPARATOR) is sent" do
    defined?(File::SEPARATOR).should == "constant"
  end

  it "returns 'method' when defined?(Object.nil?) is sent" do
    defined?(Object.nil?).should == "method"
  end

  it "returns 'expression' when defined?(0) is sent" do
    defined?(0).should == "expression"
  end

  # I (Evan) am not certain we'll support defined?(super) ever.
  # for now, i'm marking these as compliant.

  compliant_on :ruby do
    it "returns true when Bar#no_args uses defined?" do
      (!!Bar.new.no_args).should == true
    end

    it "returns true when Bar#args uses defined?" do
      (!!Bar.new.args).should == true
    end
  end
end

