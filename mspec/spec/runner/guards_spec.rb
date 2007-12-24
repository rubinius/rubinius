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

describe MSpec, ".runner?" do
  before :all do
    @verbose = $VERBOSE
    $VERBOSE = nil
  end
  
  after :all do
    $VERBOSE = @verbose
  end
  
  it "returns true when passed :mspec and ENV['MSPEC_RUNNER'] is true" do
    ENV['MSPEC_RUNNER'] = '1'
    MSpec.runner?(:mspec).should == true
  end
  
  it "returns false when passed :mspec and ENV['MSPEC_RUNNER'] is false" do
    ENV.delete 'MSPEC_RUNNER'
    MSpec.runner?(:mspec).should == false
  end
  
  it "returns true when passed :rspec and ENV['RSPEC_RUNNER'] is false but the constant Spec exists" do
    ENV.delete 'RSPEC_RUNNER'
    Object.const_set(:Spec, 1) unless Object.const_defined?(:Spec)
    MSpec.runner?(:rspec).should == true
  end

  it "returns true when passed :rspec and ENV['RSPEC_RUNNER'] is true but the constant Spec does not exist" do
    ENV['RSPEC_RUNNER'] = '1'
    Object.should_receive(:const_defined?).with(:Spec).any_number_of_times.and_return(false)
    MSpec.runner?(:rspec).should == true
  end
end

describe MSpec, ".platform?" do
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
  
  it "returns false when arg does not match RUBY_PLATFORM" do
    MSpec.platform?(:ruby).should == false
  end
  
  it "returns false when no arg matches RUBY_PLATFORM" do
    MSpec.platform?(:ruby, :jruby, :rubinius).should == false
  end
  
  it "returns true when arg matches RUBY_PLATFORM" do
    MSpec.platform?(:solarce).should == true
  end
  
  it "returns true when any arg matches RUBY_PLATFORM" do
    MSpec.platform?(:ruby, :jruby, :solarce, :rubinius).should == true
  end
end

describe MSpec, ".version?" do
  before :all do
    @verbose = $VERBOSE
    $VERBOSE = nil
  end
  
  after :all do
    $VERBOSE = @verbose
  end
  
  before :each do
    @ruby_version = Object.const_get :RUBY_VERSION
    Object.const_set :RUBY_VERSION, '1.8.5'
  end
  
  after :each do
    Object.const_set :RUBY_VERSION, @ruby_version
  end
  
  it "returns true when arg matches RUBY_VERSION" do
    MSpec.version?('1.8.5').should == true
  end
  
  it "returns true when arg is a Range and arg matches RUBY_VERSION" do
    MSpec.version?('1.8'..'1.8.6').should == true
  end
  
  it "returns true when any arg matches RUBY_VERSION" do
    MSpec.version?('1.8.4', '1.8.5', '1.8.6').should == true
  end
  
  it "returns false when arg does not match RUBY_VERSION" do
    MSpec.version?('1.8.6').should == false
  end
  
  it "returns false when arg is a Range and arg does not match RUBY_VERSION" do
    MSpec.version?('1.8.2'..'1.8.4').should == false
  end
  
  it "returns false when all arg does not match RUBY_VERSION" do
    MSpec.version?('1.8.2', '1.8.3', '1.8.4').should == false
  end
end

describe MSpec, ".date? (converting argument with Time.parse)" do
  before :all do
    @verbose = $VERBOSE
    $VERBOSE = nil
  end
  
  after :all do
    $VERBOSE = @verbose
  end
  
  before :each do
    @ruby_release_date = Object.const_get :RUBY_RELEASE_DATE
    Object.const_set :RUBY_RELEASE_DATE, '2007-10-18'
  end
  
  after :each do
    Object.const_set :RUBY_RELEASE_DATE, @ruby_release_date
  end
  
  it "returns true when arg matches RUBY_RELEASE_DATE" do
    MSpec.date?('2007-10-18').should == true
  end
  
  it "returns true when arg is a Range and arg matches RUBY_RELEASE_DATE" do
    MSpec.date?('2007-9-4'..'2007-10-20').should == true
  end
  
  it "returns true when any arg matches RUBY_RELEASE_DATE" do
    MSpec.date?('2007-9-4', '2007-10-18', '2007-10-28')
  end
  
  it "returns false when arg does not match RUBY_RELEASE_DATE" do
    MSpec.date?('2007-10-08').should == false
  end
  
  it "returns false when arg is a Range and arg does not match RUBY_RELEASE_DATE" do
    MSpec.date?('2007-9-4'..'2007-9-20').should == false
  end
  
  it "returns false when all arg does not match RUBY_RELEASE_DATE" do
    MSpec.date?('2007-9-4', '2007-10-8', '2007-10-28').should == false
  end
end

describe MSpec, ".patch?" do
  before :all do
    @verbose = $VERBOSE
    $VERBOSE = nil
  end
  
  after :all do
    $VERBOSE = @verbose
  end
  
  before :each do
    @ruby_patchlevel = Object.const_get :RUBY_PATCHLEVEL if Object.const_defined? :RUBY_PATCHLEVEL
    Object.const_set :RUBY_PATCHLEVEL, 42
  end
  
  after :each do
    Object.const_set :RUBY_PATCHLEVEL, @ruby_patchlevel
  end
  
  it "returns true when arg equals RUBY_PATCHLEVEL" do
    MSpec.patch?(42).should == true
  end
  
  it "returns true when arg is a Range and arg matches RUBY_PATCHLEVEL" do
    MSpec.patch?(40..50).should == true
  end
  
  it "returns true when any arg matches RUBY_PATCHLEVEL" do
    MSpec.patch?(38, 40, 42, 44).should == true
  end
  
  it "returns false when arg does not match RUBY_PATCHLEVEL" do
    MSpec.patch?(44).should == false
  end
  
  it "returns false when all arg does not match RUBY_PATCHLEVEL" do
    MSpec.patch?(38, 40, 44).should == false
  end
  
  it "returns false when arg is a Range and arg does not match RUBY_PATCHLEVEL" do
    MSpec.patch?(44..54).should == false
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
  
  it "does not yield when MSpec.platform? returns false" do
    lambda {
      platform(:ruby) { raise Exception, "I have not been raised" }
    }.should_not raise_error
  end
  
  it "does not yield when MSpec.platform? returns false" do
    lambda {
      platform(:ruby, :jruby, :rubinius) { raise Exception, "I have not been raised" }
    }.should_not raise_error
  end
  
  it "yields when MSpec.platform? returns true" do
    lambda {
      platform(:solarce) { raise Exception, "I am raised" }
    }.should raise_error(Exception, "I am raised")
  end
  
  it "yields when MSpec.platform? returns true" do
    lambda {
      platform(:ruby, :jruby, :solarce, :rubinius) { raise Exception, "I am raised" }
    }.should raise_error(Exception, "I am raised")
  end
end

describe Object, "#platform_not" do
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
  
  it "does not yield when MSpec.platform? returns true" do
    lambda {
      platform_not(:solarce) { raise Exception, "I have not been raised" }
    }.should_not raise_error
  end
  
  it "does not yield when MSpec.platform? returns true" do
    lambda {
      platform_not(:ruby, :jruby, :solarce, :rubinius) { raise Exception, "I have not been raised" }
    }.should_not raise_error
  end
  
  it "yields when MSpec.platform? returns false" do
    lambda {
      platform_not(:ruby) { raise Exception, "I am raised" }
    }.should raise_error(Exception, "I am raised")
  end
  
  it "yields when MSpec.platform? returns false" do
    lambda {
      platform_not(:ruby, :jruby, :rubinius) { raise Exception, "I am raised" }
    }.should raise_error(Exception, "I am raised")
  end
end

describe Object, "#platform :version => VERSION_SPEC" do
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
  
  it "does not yield when MSpec.version? returns false" do
    lambda {
      platform(:version => '1.6.2'..'1.6.4') { raise Exception, "I have not been raised" }
    }.should_not raise_error
  end
  
  it "does not yield when MSpec.version? returns false" do
    lambda {
      platform(:version => '1.8.4') { raise Exception, "I have not been raised" }
    }.should_not raise_error
  end
  
  it "does not yield when MSpec.version? returns false" do
    lambda {
      platform(:version => ['1.8.4', '1.8.5', '1.8.7']) { raise Exception, "I have not been raised" }
    }.should_not raise_error
  end
  
  it "yields when arg MSpec.version? returns true" do
    lambda {
      platform(:version => '1.8.2'..'1.8.6') { raise Exception, "I am raised" }
    }.should raise_error(Exception, "I am raised")
  end

  it "yields when arg MSpec.version? returns true" do
    lambda {
      platform(:version => '1.8.6') { raise Exception, "I am raised" }
    }.should raise_error(Exception, "I am raised")
  end

  it "yields when arg MSpec.version? returns true" do
    lambda {
      platform(:version => ['1.8.2', '1.8.4', '1.8.6']) { raise Exception, "I am raised" }
    }.should raise_error(Exception, "I am raised")
  end
end

describe Object, "#platform_not :version => VERSION_SPEC" do
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

  it "does not yield when MSpec.version? returns true" do
    lambda {
      platform_not(:version => '1.8.2'..'1.8.6') { raise Exception, "I have not been raised" }
    }.should_not raise_error
  end
  
  it "does not yield when MSpec.version? returns true" do
    lambda {
      platform_not(:version => '1.8.6') { raise Exception, "I have not been raised" }
    }.should_not raise_error
  end
  
  it "does not yield when MSpec.version? returns true" do
    lambda {
      platform_not(:version => ['1.8.4', '1.8.6', '1.8.7']) { raise Exception, "I have not been raised" }
    }.should_not raise_error
  end
  
  it "yields when MSpec.version? returns false" do
    lambda {
      platform_not(:version => '1.6.2'..'1.6.4') { raise Exception, "I am raised" }
    }.should raise_error(Exception, "I am raised")
  end

  it "yields when arg MSpec.version? returns false" do
    lambda {
      platform_not(:version => '1.8.5') { raise Exception, "I am raised" }
    }.should raise_error(Exception, "I am raised")
  end

  it "yields when arg MSpec.version? returns false" do
    lambda {
      platform_not(:version => ['1.8.2', '1.8.4', '1.8.5']) { raise Exception, "I am raised" }
    }.should raise_error(Exception, "I am raised")
  end
end

describe Object, "#platform :date => DATE_SPEC" do
  before :all do
    @verbose = $VERBOSE
    $VERBOSE = nil
  end
  
  after :all do
    $VERBOSE = @verbose
  end
  
  before :each do
    @ruby_release_date = Object.const_get :RUBY_RELEASE_DATE
    Object.const_set :RUBY_RELEASE_DATE, "2007-10-18"
  end
  
  after :each do
    Object.const_set :RUBY_RELEASE_DATE, @ruby_release_date
  end
  
  it "does not yield when MSpec.date? returns false" do
    lambda {
      platform(:date => '2006-10-18'..'2006-10-19') { raise Exception, "I have not been raised" }
    }.should_not raise_error
  end
  
  it "does not yield when MSpec.date? returns false" do
    lambda {
      platform(:date => '2007-10-12') { raise Exception, "I have not been raised" }
    }.should_not raise_error
  end
  
  it "does not yield when MSpec.date? returns false" do
    lambda {
      platform(:date => ['2007-10-14', '2007-10-16', '2007-10-19']) { raise Exception, "I have not been raised" }
    }.should_not raise_error
  end
  
  it "yields when MSpec.date? returns true" do
    lambda {
      platform(:date => '2007-10-14'..'2007-10-20') { raise Exception, "I am raised" }
    }.should raise_error(Exception, "I am raised")
  end

  it "yields when arg MSpec.date? returns true" do
    lambda {
      platform(:date => '2007-10-18') { raise Exception, "I am raised" }
    }.should raise_error(Exception, "I am raised")
  end

  it "yields when arg MSpec.date? returns true" do
    lambda {
      platform(:date => ['2007-10-14', '2007-10-18', '2007-10-19']) { raise Exception, "I am raised" }
    }.should raise_error(Exception, "I am raised")
  end
end

describe Object, "#platform_not :date => DATE_SPEC" do
  before :all do
    @verbose = $VERBOSE
    $VERBOSE = nil
  end
  
  after :all do
    $VERBOSE = @verbose
  end
  
  before :each do
    @ruby_release_date = Object.const_get :RUBY_RELEASE_DATE
    Object.const_set :RUBY_RELEASE_DATE, "2007-10-18"
  end
  
  after :each do
    Object.const_set :RUBY_RELEASE_DATE, @ruby_release_date
  end
  
  it "does not yield when MSpec.date? returns true" do
    lambda {
      platform_not(:date => '2007-10-14'..'2007-10-20') { raise Exception, "I have not been raised" }
    }.should_not raise_error
  end
  
  it "does not yield when MSpec.date? returns true" do
    lambda {
      platform_not(:date => '2007-10-18') { raise Exception, "I have not been raised" }
    }.should_not raise_error
  end
  
  it "does not yield when MSpec.date? returns true" do
    lambda {
      platform_not(:date => ['2007-10-14', '2007-10-18', '2007-10-19']) { raise Exception, "I have not been raised" }
    }.should_not raise_error
  end
  
  it "yields when MSpec.date? returns false" do
    lambda {
      platform_not(:date => '2006-10-14'..'2006-10-17') { raise Exception, "I am raised" }
    }.should raise_error(Exception, "I am raised")
  end

  it "yields when arg MSpec.date? returns false" do
    lambda {
      platform_not(:date => '2007-10-19') { raise Exception, "I am raised" }
    }.should raise_error(Exception, "I am raised")
  end

  it "yields when arg MSpec.date? returns false" do
    lambda {
      platform_not(:date => ['2007-10-14', '2007-10-16', '2007-10-19']) { raise Exception, "I am raised" }
    }.should raise_error(Exception, "I am raised")
  end
end

describe Object, "#platform :patch => PATCHLEVEL_SPEC" do
  before :all do
    @verbose = $VERBOSE
    $VERBOSE = nil
  end
  
  after :all do
    $VERBOSE = @verbose
  end
  
  before :each do
    @ruby_patchlevel = Object.const_get :RUBY_PATCHLEVEL
    Object.const_set :RUBY_PATCHLEVEL, 121
  end
  
  after :each do
    Object.const_set :RUBY_PATCHLEVEL, @ruby_patchlevel
  end
  
  it "does not yield when MSpec.patch? returns false" do
    lambda {
      platform(:patch => 110..120) { raise Exception, "I have not been raised" }
    }.should_not raise_error
  end
  
  it "does not yield when MSpec.patch? returns false" do
    lambda {
      platform(:patch => 112) { raise Exception, "I have not been raised" }
    }.should_not raise_error
  end
  
  it "does not yield when MSpec.patch? returns false" do
    lambda {
      platform(:patch => [110, 112, 114]) { raise Exception, "I have not been raised" }
    }.should_not raise_error
  end
  
  it "yields when MSpec.patch? returns true" do
    lambda {
      platform(:patch => 120..130) { raise Exception, "I am raised" }
    }.should raise_error(Exception, "I am raised")
  end

  it "yields when arg MSpec.patch? returns true" do
    lambda {
      platform(:patch => 121) { raise Exception, "I am raised" }
    }.should raise_error(Exception, "I am raised")
  end

  it "yields when arg MSpec.patch? returns true" do
    lambda {
      platform(:patch => [120, 121, 122]) { raise Exception, "I am raised" }
    }.should raise_error(Exception, "I am raised")
  end
end

describe Object, "#platform_not :patch => PATCHLEVEL_SPEC" do
  before :all do
    @verbose = $VERBOSE
    $VERBOSE = nil
  end
  
  after :all do
    $VERBOSE = @verbose
  end
  
  before :each do
    @ruby_patchlevel = Object.const_get :RUBY_PATCHLEVEL
    Object.const_set :RUBY_PATCHLEVEL, 121
  end
  
  after :each do
    Object.const_set :RUBY_PATCHLEVEL, @ruby_patchlevel
  end
  
  it "does not yield when MSpec.patch? returns true" do
    lambda {
      platform_not(:patch => 118..122) { raise Exception, "I have not been raised" }
    }.should_not raise_error
  end
  
  it "does not yield when MSpec.patch? returns true" do
    lambda {
      platform_not(:patch => 121) { raise Exception, "I have not been raised" }
    }.should_not raise_error
  end
  
  it "does not yield when MSpec.patch? returns true" do
    lambda {
      platform_not(:patch => [110, 120, 121]) { raise Exception, "I have not been raised" }
    }.should_not raise_error
  end
  
  it "yields when MSpec.patch? returns false" do
    lambda {
      platform_not(:patch => 110..120) { raise Exception, "I am raised" }
    }.should raise_error(Exception, "I am raised")
  end

  it "yields when arg MSpec.patch? returns false" do
    lambda {
      platform_not(:patch => 112) { raise Exception, "I am raised" }
    }.should raise_error(Exception, "I am raised")
  end

  it "yields when arg MSpec.patch? returns false" do
    lambda {
      platform_not(:patch => [120, 122, 124]) { raise Exception, "I am raised" }
    }.should raise_error(Exception, "I am raised")
  end
end

describe Object, "#runner" do
  before :all do
    @verbose = $VERBOSE
    $VERBOSE = nil
  end
  
  after :all do
    $VERBOSE = @verbose
  end
  
  it "yields when passed :mspec and ENV['MSPEC_RUNNER'] is true" do
    ENV['MSPEC_RUNNER'] = '1'
    lambda {
      runner(:mspec) { raise Exception, "I am raised" }
    }.should raise_error(Exception, "I am raised")
  end
  
  it "does not yield when passed :mspec and ENV['MSPEC_RUNNER'] is false" do
    ENV.delete 'MSPEC_RUNNER'
    lambda {
      runner(:mspec) { raise Exception, "I have not been raised" }
    }.should_not raise_error
  end
  
  it "yields when passed :rspec and ENV['RSPEC_RUNNER'] is true" do
    ENV['RSPEC_RUNNER'] = '1'
    lambda {
      runner(:rspec) { raise Exception, "I am raised" }
    }.should raise_error(Exception, "I am raised")
  end

  it "yields when passed :rspec and the constant Spec exists" do
    ENV.delete 'RSPEC_RUNNER'
    Object.const_set(:Spec, 1) unless Object.const_defined?(:Spec)
    lambda {
      runner(:rspec) { raise Exception, "I am raised" }
    }.should raise_error(Exception, "I am raised")
  end
  
  it "does not yield when passed :rspec and ENV['RSPEC_RUNNER'] is false and the constant Spec does not exist" do
    ENV.delete 'RSPEC_RUNNER'
    Object.should_receive(:const_defined?).with(:Spec).and_return(false)
    lambda {
      runner(:rspec) { raise Exception, "I have not been raised" }
    }.should_not raise_error
  end
end

describe Object, "#runner_not" do
  before :all do
    @verbose = $VERBOSE
    $VERBOSE = nil
  end
  
  after :all do
    $VERBOSE = @verbose
  end
  
  it "does not yield when passed :mspec and ENV['MSPEC_RUNNER'] is true" do
    ENV['MSPEC_RUNNER'] = '1'
    lambda {
      runner_not(:mspec) { raise Exception, "I have not been raised" }
    }.should_not raise_error
  end
  
  it "yields when passed :mspec and ENV['MSPEC_RUNNER'] is false" do
    ENV.delete 'MSPEC_RUNNER'
    lambda {
      runner_not(:mspec) { raise Exception, "I am raised" }
    }.should raise_error(Exception, "I am raised")
  end
  
  it "does not yield when passed :rspec and ENV['RSPEC_RUNNER'] is false but the constant Spec exists" do
    ENV.delete 'RSPEC_RUNNER'
    Object.const_set(:Spec, 1) unless Object.const_defined?(:Spec)
    lambda {
      runner_not(:rspec) { raise Exception, "I have not been raised" }
    }.should_not raise_error
  end

  it "does not yield when passed :rspec and ENV['RSPEC_RUNNER'] is true but the constant Spec does not exist" do
    ENV['RSPEC_RUNNER'] = '1'
    Object.should_receive(:const_defined?).with(:Spec).any_number_of_times.and_return(false)
    lambda {
      runner_not(:rspec) { raise Exception, "I have not been raised" }
    }.should_not raise_error
  end
end
