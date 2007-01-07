require 'rubygems'
require 'spec'
require File.dirname(__FILE__) + '/../mri_target'

class MRITargetSpec
  include MRITarget
end

context "MRITarget" do
  setup do
    @target = MRITargetSpec.new
  end
  
  specify "should provide make_cache_directory method" do
    @target.should_respond_to :make_cache_directory
  end

  specify "should provide rubinius_path method" do
    @target.should_respond_to :rubinius_path
  end

  specify "rubinius_path should be the root directory" do
    @target.rubinius_path.should == File.expand_path(File.dirname(__FILE__) + '/../..')
  end

  specify "should provide cache_path method" do
    @target.should_respond_to :cache_path
  end

  specify "cache_path should be under rubinius_path" do
    @target.cache_path.should == @target.rubinius_path + '/code-cache'
  end

  specify "should provide example method" do
    @target.should_respond_to :example
  end

  specify "example should take a default argument" do
    @target.method(:example).arity.should == -1
  end

  specify "example should receive a block" do
    lambda { @target.example }.should_raise ArgumentError
  end

  specify "should provide template method" do
    @target.should_respond_to :template
  end

  specify "template should provide Ruby source wrapper for calling example method in Rubinius" do
    @target.template.should == <<-CODE
%s
%s
RubiniusSpecExample.new.__example__
CODE
  end

  specify "should provide cache_source_name method" do
    @target.should_respond_to :cache_source_name
  end

  specify "cache_source_name should build name of source file in code-cache directory" do
    @target.stub!(:caller).and_return(caller_stub)
    @target.stub!(:cache_path).and_return("/Users/rubinius/code-cache")
    @target.cache_source_name(source_stub).should == 
      "/Users/rubinius/code-cache/mri_spec-68-16000475-mri.rb"
  end

  specify "should provide caller_name method" do
    @target.should_respond_to :caller_name
  end

  specify "caller_name should set the name of the calling spec file and line number" do
    @target.stub!(:source).and_return(source_stub)
    @target.stub!(:caller).and_return(caller_stub)
    @target.caller_name.should == "mri_spec-68"
  end

  specify "should provide source method" do
    @target.should_respond_to :source
  end

  specify "source should return file path" do
    @target.stub!(:caller).and_return(caller_stub)
    @target.stub!(:cache_path).and_return("/Users/rubinius/code-cache")
    @target.source('') { [1, 2, 3] }.should ==
      "/Users/rubinius/code-cache/mri_spec-68-890456808-mri.rb"
  end

  specify "should provide execute method" do
    @target.should_respond_to :execute
  end

  private
  def source_stub
    <<-CODE
    class SomeClass
      def boo!
        puts 'aiiya'
      end
    end
    CODE
  end

  def caller_stub
     [ "./spec/targets/../mri_target.rb:46:in `cache_source_name'",
       "./spec/targets/../mri_target.rb:42:in `save_source'",
       "./spec/targets/../mri_target.rb:23:in `example'",
       "./spec/targets/mri_spec.rb:68",
       "/opt/local/lib/ruby/gems/1.8/gems/rspec-0.7.5/lib/spec/runner/specification.rb:55:in `execute_spec'"
     ]
  end
end
