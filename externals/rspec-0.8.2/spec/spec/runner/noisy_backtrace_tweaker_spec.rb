require File.dirname(__FILE__) + '/../../spec_helper.rb'

module Spec
  module Runner
    context "NoisyBacktraceTweaker" do
      setup do
        @error = RuntimeError.new
        @tweaker = NoisyBacktraceTweaker.new
      end
        
      specify "should leave anything in lib spec dir" do
        ["expectations", "mocks", "runner", "stubs"].each do |child|
          @error.set_backtrace(["/lib/spec/#{child}/anything.rb"])
          @tweaker.tweak_backtrace(@error, "spec name")
          @error.backtrace.should_not_be_empty
        end
      end

      specify "should leave anything in spec dir" do
        @error.set_backtrace(["/lib/spec/expectations/anything.rb"])
        @tweaker.tweak_backtrace(@error, "spec name")
        @error.backtrace.should_not_be_empty
      end

      specify "should leave bin spec" do
        @error.set_backtrace(["bin/spec:"])
        @tweaker.tweak_backtrace(@error, "spec name")
        @error.backtrace.should_not_be_empty
      end

      specify "should not barf on nil backtrace" do
        lambda do
          @tweaker.tweak_backtrace(@error, "spec name")
        end.should_not_raise
      end
      
      specify "should clean up double slashes" do
        @error.set_backtrace(["/a//b/c//d.rb"])
        @tweaker.tweak_backtrace(@error, "spec name")
        @error.backtrace.should_include "/a/b/c/d.rb"
      end
      
    end
  end
end