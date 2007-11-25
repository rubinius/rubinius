require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/../../runner/guards'

describe MSpec, ".engine?" do
  before :all do
    @verbose = $VERBOSE
    $VERBOSE = nil
  end
  
  after :all do
    $VERBOSE = @verbose
  end
  
  before :each do
    @ruby_name = Object.const_get :RUBY_NAME
  end
  
  after :each do
    Object.const_set :RUBY_NAME, @ruby_name
  end
  
  it "returns true if passed :ruby and RUBY_NAME == 'ruby'" do
    Object.const_set :RUBY_NAME, 'ruby'
    MSpec.engine?(:ruby).should == true
  end
  
  it "returns true if passed :mri and RUBY_NAME == 'ruby'" do
    Object.const_set :RUBY_NAME, 'ruby'
    MSpec.engine?(:mri).should == true
  end
  
  it "returns true if passed :rbx and RUBY_NAME == 'rbx'" do
    Object.const_set :RUBY_NAME, 'rbx'
    MSpec.engine?(:rbx).should == true
  end
  
  it "returns true if passed :rubinius and RUBY_NAME == 'rbx'" do
    Object.const_set :RUBY_NAME, 'rbx'
    MSpec.engine?(:rubinius).should == true
  end
  
  it "returns true if passed :jruby and RUBY_NAME == 'jruby'" do
    Object.const_set :RUBY_NAME, 'jruby'
    MSpec.engine?(:jruby).should == true
  end
  
  it "returns false when passed an unrecognized name" do
    Object.const_set :RUBY_NAME, 'ruby'
    MSpec.engine?(:python).should == false
  end
end

describe MSpec, ".guard?" do
  it "accepts multiple arguments and passes them to the block" do
    a = []
    MSpec.guard?(:a, :b) { |i| a << i; false }
    a.should == [:a, :b]
  end
  
  it "returns true when the block evaluates to true" do
    MSpec.guard? { true }.should == true
    MSpec.guard?(:a, :b) { |x| x == :b }.should == true
  end
  
  it "returns false when the block evaluates to false" do
    MSpec.guard? { false }.should == false
    MSpec.guard?(:a, :b) { |x| x == :c }.should == false
  end
end

describe Object, "#failure" do
  it "does not yield when MSpec.engine? returns true" do
    MSpec.should_receive(:engine?).with(:rbx).and_return(true)
    lambda {
      failure(:rbx) { raise Exception, "I have not been raised" }
    }.should_not raise_error
  end
  
  it "yields when MSpec.engine? returns false" do
    MSpec.should_receive(:engine?).and_return(false)
    lambda {
      failure(:rbx) { raise Exception, "I am raised" }
    }.should raise_error(Exception, "I am raised")
  end
end

describe Object, "#extension" do
  it "does not yield when MSpec.engine? returns false" do
    MSpec.should_receive(:engine?).with(:rbx).and_return(false)
    lambda {
      extension(:rbx) { raise Exception, "I have not been raised" }
    }.should_not raise_error
  end
  
  it "yields when MSpec.engine? returns true" do
    MSpec.should_receive(:engine?).and_return(true)
    lambda {
      extension(:rbx) { raise Exception, "I am raised" }
    }.should raise_error(Exception, "I am raised")
  end
end

describe Object, "#compliant" do
  it "does not yield when MSpec.engine? returns false" do
    MSpec.should_receive(:engine?).with(:rbx).and_return(false)
    lambda {
      compliant(:rbx) { raise Exception, "I have not been raised" }
    }.should_not raise_error
  end
  
  it "yields when MSpec.engine? returns true" do
    MSpec.should_receive(:engine?).and_return(true)
    lambda {
      compliant(:rbx) { raise Exception, "I am raised" }
    }.should raise_error(Exception, "I am raised")
  end
end

describe Object, "#compliant" do
  it "does not yield when MSpec.engine? returns false" do
    MSpec.should_receive(:engine?).with(:rbx).and_return(false)
    lambda {
      noncompliant(:rbx) { raise Exception, "I have not been raised" }
    }.should_not raise_error
  end
  
  it "yields when MSpec.engine? returns true" do
    MSpec.should_receive(:engine?).and_return(true)
    lambda {
      noncompliant(:rbx) { raise Exception, "I am raised" }
    }.should raise_error(Exception, "I am raised")
  end
end

describe Object, "#version" do
  before :all do
    @verbose = $VERBOSE
    $VERBOSE = nil
  end
  
  after :all do
    $VERBOSE = @verbose
  end
  
  before :each do
    @ruby_version = Object.const_get :RUBY_VERSION
    Object.const_set :RUBY_VERSION, "1.8.6"
  end
  
  after :each do
    Object.const_set :RUBY_VERSION, @ruby_version
  end
  
  it "does not yield when arg === RUBY_VERSION returns false" do
    lambda {
      version('1.6.2'..'1.6.4') { raise Exception, "I have not been raised" }
    }.should_not raise_error
  end
  
  it "does not yield when passed :not and arg === RUBY_VERSION returns true" do
    lambda {
      version(:not, '1.8.2'..'1.8.6') { raise Exception, "I have not been raised" }
    }.should_not raise_error
  end
  
  it "yields when arg === RUBY_VERSION returns true" do
    lambda {
      version('1.8.2'..'1.8.6') { raise Exception, "I am raised" }
    }.should raise_error(Exception, "I am raised")
  end
  
  it "yields when passed :not and arg === RUBY_VERSION returns false" do
    lambda {
      version(:not, '1.6.2'..'1.6.4') { raise Exception, "I am raised" }
    }.should raise_error(Exception, "I am raised")
  end
end

describe Object, "#platform" do
  before :all do
    @verbose = $VERBOSE
    $VERBOSE = nil
  end
  
  after :all do
    $VERBOSE = @verbose
  end
  
  before :each do
    @ruby_platform = Object.const_get :RUBY_PLATFORM
    Object.const_set :RUBY_PLATFORM, 'solarce'
  end
  
  after :each do
    Object.const_set :RUBY_PLATFORM, @ruby_platform
  end
  
  it "does not yield when arg does not match RUBY_PLATFORM" do
    lambda {
      platform(:ruby) { raise Exception, "I have not been raised" }
    }.should_not raise_error
  end
  
  it "does not yield when passed :not and arg matches RUBY_PLATFORM" do
    lambda {
      platform(:not, :solarce) { raise Exception, "I have not been raised" }
    }.should_not raise_error
  end
  
  it "yields when arg matches RUBY_PLATFORM" do
    lambda {
      platform(:solarce) { raise Exception, "I am raised" }
    }.should raise_error(Exception, "I am raised")
  end
  
  it "yields when passed :not and arg does not match RUBY_PLATFORM" do
    lambda {
      platform(:not, :ruby) { raise Exception, "I am raised" }
    }.should raise_error(Exception, "I am raised")
  end
end
