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
class IO
  alias :native_write :write
end

def STDOUT.write(str)
  @output ||= []
  @output << str
end
    
def STDOUT.output
  @output
end

result = lambda do
  %s
  %s
end.call

STDOUT.native_write "[ "
result = result.inspect
result = result.inspect if /^#\<.+\>/.match(result)
STDOUT.native_write result
STDOUT.native_write ", "
STDOUT.native_write STDOUT.output.inspect
STDOUT.native_write " ]"
CODE
  end
  
  specify "should provide a code method" do
    @target.should_respond_to :code
  end
  
  specify "should provide cache_source_name method" do
    @target.should_respond_to :cache_source_name
  end
  
  specify "cache_source_name should build name of source file in code-cache directory" do
    @target.stub!(:caller).and_return(caller_stub)
    @target.stub!(:cache_path).and_return("/Users/rubinius/code-cache")
    @target.cache_source_name(source_stub).should_match(
      %r!/Users/rubinius/code-cache/rubinius_spec-68-\d*.rb!)
  end
  
  specify "should provide caller_name method" do
    @target.should_respond_to :caller_name
  end
  
  specify "caller_name should set the name of the calling spec file and line number" do
    @target.stub!(:source).and_return(source_stub)
    @target.stub!(:caller).and_return(caller_stub)
    @target.caller_name.should == "rubinius_spec-68"
  end
  
  specify "should provide source method" do
    @target.should_respond_to :source
  end
  
  specify "source should return file path" do
    @target.stub!(:caller).and_return(caller_stub)
    @target.stub!(:cache_path).and_return("/Users/rubinius/code-cache")
    @target.source('') { [1, 2, 3] }.should_match(
      %r!/Users/rubinius/code-cache/rubinius_spec-68-\d*.rb!)
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

require File.dirname(__FILE__) + '/../spec_helper'
context "Rubinius target" do
  specify "should allow to get the strings written STDOUT" do
    example do
      puts "hola"
      puts "space is significant in this heredoc"
      puts "unindent removes the first blanks found on the first line"
      puts "on each of these lines"
      puts "adios"
    end.stdout.should == <<-OUT.unindent
      hola
      space is significant in this heredoc
      unindent removes the first blanks found on the first line
      on each of these lines
      adios
    OUT
    end

  specify "should allow to get the lines written to STDOUT" do
    example do
      puts "hello"
    end.stdout_lines.length == 1

    example do
      print "bye"
    end.stdout_lines.first.should == "bye"
  end

  specify "should allow to access the evaluation result along with STDOUT" do
    result = example do
      puts 42
      "The answer is"
    end

    result.should == "The answer is"
    result.stdout_lines.should == ["42\n"]
  end


end


