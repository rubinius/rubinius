require 'rubygems'
require 'spec'
require File.dirname(__FILE__) + '/../rubinius_target'

class RubiniusTargetSpec
  include RubiniusTarget
end

context "RubiniusTarget" do
  setup do
    @target = RubiniusTargetSpec.new
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
    @target.cache_source_name(source_stub).should == "/Users/rubinius/code-cache/rubinius_spec-68-16000475.rb"
  end
  
  specify "should provide caller_name method" do
    @target.should_respond_to :caller_name
  end
  
  specify "caller_name should set the name of the calling spec file and line number" do
    @target.stub!(:source).and_return(source_stub)
    @target.stub!(:caller).and_return(caller_stub)
    @target.caller_name.should == "rubinius_spec-68"
  end
  
  specify "should provide compile method" do
    @target.should_respond_to :compile
  end
  
  specify "compile should return compiled file path" do
    @target.stub!(:caller).and_return(caller_stub)
    @target.stub!(:cache_path).and_return("/Users/rubinius/code-cache")
    Object.send(:remove_const, :Machine)
    Machine = mock("Machine", :null_object => true)
    Machine.stub!(:compile_file)
    @target.compile('') { [1, 2, 3] }.should == 
      "/Users/rubinius/code-cache/rubinius_spec-68-890456808.rbc"
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
     [ "./spec/targets/../rubinius_target.rb:46:in `cache_source_name'",
       "./spec/targets/../rubinius_target.rb:42:in `save_source'",
       "./spec/targets/../rubinius_target.rb:23:in `example'",
       "./spec/targets/rubinius_spec.rb:68",
       "/opt/local/lib/ruby/gems/1.8/gems/rspec-0.7.5/lib/spec/runner/specification.rb:55:in `execute_spec'"
     ]
  end
end
