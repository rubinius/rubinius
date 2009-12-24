require File.dirname(__FILE__) + '/../../spec_helper'

$load_fixture_dir = (File.dirname(__FILE__) + '/../../fixtures/load')
$LOAD_PATH << $load_fixture_dir

$load_spec_1 = nil
$load_spec_2 = nil
$load_spec_3 = nil
$load_spec_4 = nil
$load_spec_5 = nil
$load_spec_6 = nil
$load_spec_7 = nil
$load_spec_8 = nil
$load_spec_9 = nil
$load_spec_10 = nil
$load_spec_wrap   = nil
$load_spec_wrap2  = nil
$load_spec = nil
$load_spec_rooby = nil

require 'rbconfig'

# The files used below just contain code that assigns
# Time.now to the respective global variable.
#
# The only exception are the _wrap files which contain
# similarly named classes. The class definitions, in
# addition, contain another global timestamp assignment.
describe "Kernel#load" do
  it "is a private method" do
    Kernel.should have_private_instance_method(:load)
  end

  it "loads a .rb from an absolute path and returns true" do
    path = File.expand_path(File.dirname(__FILE__) + '/../../fixtures/load/load_spec_1.rb')

    load(path).should == true
    $load_spec_1.nil?.should == false
  end

  it "loads a .rb from a relative path and returns true" do
    Dir.chdir($load_fixture_dir) do |dir|
      $load_spec_1 = nil
      load('../../fixtures/load/load_spec_1.rb').should == true
      $load_spec_1.nil?.should == false

      $load_spec_1 = nil
      load('./../load/load_spec_1.rb').should == true
      $load_spec_1.nil?.should == false
    end
  end

  # TODO: add an implementation-agnostic method for creating
  # an extension file
  it "loads extension files"

  it "loads an unqualified .rb by looking in $LOAD_PATH and returns true" do
    load('load_spec_2.rb').should == true
    $load_spec_2.nil?.should == false
  end

  it "does not expand/resolve qualified files against $LOAD_PATH" do
    Dir.chdir($load_fixture_dir + '/../') do |dir|
      # This would be a valid path if expanded against the fixture dir
      lambda { load '../load/load_spec_2.rb' }.should raise_error LoadError
    end
  end

  it "allows unqualified files to contain path information (just not in the beginning)" do
    name = (File.dirname(__FILE__) + '/../../fixtures')
    $LOAD_PATH << name

    $load_spec_2 = nil
    load('load/../load/load_spec_2.rb').should == true
    $load_spec_2.nil?.should == false
    
    $LOAD_PATH.delete name
  end

  it "loads a file with no extension as a ruby source file" do
    load('load_spec').should == true
    $load_spec.nil?.should == false
  end

  it "loads a file with any extension as a ruby source file" do
    load('load_spec.rooby').should == true
    $load_spec_rooby.nil?.should == false
  end

  it "does not create a .rb file for the non-.rb files" do
    load('load_spec').should == true
    load('load_spec.rooby').should == true

    File.exist?("#{$load_fixture_dir}/load_spec.rb").should == false
    File.exist?("#{$load_fixture_dir}/load_spec.rooby.rb").should == false
  end

  it "does not add the loaded file to $LOADED_FEATURES" do
    $LOADED_FEATURES.grep(/load_spec_3.rb/).should == []
    load('load_spec_3.rb').should == true
    $load_spec_3.nil?.should == false
    $LOADED_FEATURES.grep(/load_spec_3.rb/).should == []
  end

  ruby_version_is ""..."1.9" do
    it "returns __FILE__ as a relative path" do
      Dir.chdir($load_fixture_dir) do |dir|
        load('load_spec_4.rb') 
        $load_spec_4.first.first.should == './load_spec_4.rb'
      end
    end
  end

  ruby_version_is "1.9" do
    it "returns __FILE__ as an absolute path" do
      Dir.chdir($load_fixture_dir) do |dir|
        load('load_spec_4.rb') 
        $load_spec_4.first.first.should == File.expand_path('./load_spec_4.rb')
      end
    end
  end

  it "produces __FILE__ as the given filename and __LINE__ as the source line number" do
    Dir.chdir($load_fixture_dir) do |dir|
      load('load_spec_4.rb').should == true 
      $load_spec_4.first.first.should =~ /load_spec_4\.rb$/
      $load_spec_4.first.last.should == 1
      $load_spec_4.last.first.should =~ /load_spec_4\.rb$/
      $load_spec_4.last.last.should == 10

      extended_on :rubinius do
        `rm load_spec_4.rbc`
      end
    end

    load("#{$load_fixture_dir}/load_spec_4.rb").should == true
    $load_spec_4[0][0].should =~ %r[^.*/fixtures/load/load_spec_4.rb]
    $load_spec_4[0][1].should == 1
    $load_spec_4[1][0].should =~ %r[^.*/fixtures/load/load_spec_4.rb]
    $load_spec_4[1][1].should == 10
  end

  it "reloads the file if invoked on the same filename again, returning true" do
    load('load_spec_4.rb').should == true
    load('load_spec_4.rb').should == true
  end

  it "re-evaluates the file each time it is loaded" do
    load('load_spec_5.rb').should == true
    a = $load_spec_5
    load('load_spec_5.rb').should == true
    b = $load_spec_5

    a.should_not eql(b)
  end

  it "loads the file even if it has already been #required" do
    require('load_spec_6.rb').should == true
    a = $load_spec_6

    require('load_spec_6.rb').should == false
    b = $load_spec_6

    load('load_spec_6.rb').should == true
    c = $load_spec_6

    a.should eql(b)
    c.should_not eql(a)
  end

  it "does not cause #require on the same filename to fail" do 
    load('load_spec_7.rb').should == true
    a = $load_spec_7

    require('load_spec_7.rb').should == true
    b = $load_spec_7

    load('load_spec_7.rb').should == true
    c = $load_spec_7

    a.should_not eql(b)
    b.should_not eql(c)
    c.should_not eql(a)
  end

  it "raises a LoadError if the file can't be found" do
    lambda { load("./nonexistent#{Time.now.to_f}#{$$}") }.should raise_error LoadError
  end

  # TODO: This currently fails on Rubinius because the #load logic
  #       will take 'load_spec_1' and try 'load_spec_1.rbc' since
  #       that is the logic for .rb files. 
  it "raises a LoadError if filename given without its extension" do
    lambda { load('load_spec_1') }.should raise_error LoadError
    load('load_spec_1.rb').should == true
    lambda { load('load_spec_1') }.should raise_error LoadError
  end

  it "only accepts strings as the filename argument" do
    lambda { load(nil) }.should raise_error TypeError
    lambda { load(42) }.should raise_error TypeError
    lambda { load([]) }.should raise_error TypeError
  end

  ruby_version_is "1.9" do
    it "calls #to_path on non-String arguments" do
      p = mock('path')
      p.should_receive(:to_path).and_return 'load_spec_1.rb'
      load(p)
    end
  end

  runner_is_not :rspec do
    it "allows wrapping the code in the file in an anonymous module" do
      defined?(LoadSpecWrap).should == nil
      defined?(LoadSpecWrapTwo).should == nil

      load('load_spec_wrap.rb').should == true
      $load_spec_wrap.nil?.should == false
      LoadSpecWrap.lsw.should == :lsw

      load('load_spec_wrap2.rb', true).should == true
      $load_spec_wrap2.nil?.should == false
      defined?(LoadSpecWrapTwo).should == nil
    end
  end
end

describe "Shell expansion in Kernel#load" do
  before :all do
    @rs_home = ENV["HOME"]
    ENV["HOME"] = $load_fixture_dir
    @rs_short = "~/load_spec_1.rb"
    @rs_long  = "#{$load_fixture_dir}/load_spec_1.rb"
  end

  after :all do
    ENV["HOME"] = @rs_home
  end

  before :each do
    $LOADED_FEATURES.delete @rs_long
    $LOADED_FEATURES.delete @rs_short
  end

  it "expands a preceding ~/ to the user's home directory to use as path" do
    $load_spec_1 = nil
    load(@rs_short).should == true
    $load_spec_1.nil?.should == false
  end
end

describe "Kernel.load" do
  it "needs to be reviewed for spec completeness"
end
