require File.dirname(__FILE__) + '/../../spec_helper.rb'

module Spec
  module Runner
    module SpecMatcherSpecHelper
      class MatchDescription
        def initialize(description)
          @description = description
        end
        
        def matches?(matcher)
          matcher.matches?(@description)
        end
        
        def failure_message
          "expected matcher.matches?(#{@description.inspect}) to return true, got false"
        end
        
        def negative_failure_message
          "expected matcher.matches?(#{@description.inspect}) to return false, got true"
        end
      end
      def match_description(description)
        MatchDescription.new(description)
      end
    end
        
    context "SpecMatcher" do
      include SpecMatcherSpecHelper
      
      specify "should match correct context and spec" do
        matcher=SpecMatcher.new("context", "spec")
        matcher.should match_description("context spec")
      end
      
      specify "should not match wrong spec" do
        matcher=SpecMatcher.new("context", "other spec")
        matcher.should_not match_description("context spec")
      end
      
      specify "should not match wrong context" do
        matcher=SpecMatcher.new("other context", "spec")
        matcher.should_not match_description("context spec")
      end
      
      specify "should match spec only" do
        matcher=SpecMatcher.new("context", "spec")
        matcher.should match_description("spec")
      end
      
      specify "should match context only" do
        matcher=SpecMatcher.new("context", "spec")
        matcher.should match_description("context")
      end
      
      specify "should escape regexp chars" do
        matcher=SpecMatcher.new("(con|text)", "[spec]")
        matcher.should_not match_description("con p")
      end
      
      # specify "should match context only" do
      #   matcher=SpecMatcher.new("context", "a context")
      #   matcher.should match_description("a context")
      # end
      # 
      # specify "should match spec only" do
      #   matcher=SpecMatcher.new("with a spec", "a context")
      #   matcher.should match_description("with a spec")
      # end
      
      # specify "should match with regexp reserved (characters) in the spec" do
      #   matcher=SpecMatcher.new("with ([#]) a spec", "a context")
      #   matcher.should match_description("a context")
      #   matcher.should match_description("with ([#]) a spec")
      #   matcher.should match_description("a context with ([#]) a spec")
      # end
      # 
      # specify "should match with regexp reserved (characters) in the context" do
      #   matcher=SpecMatcher.new("with a spec", "a ([#]) context")
      #   matcher.should match_description("a ([#]) context with a spec")
      #   matcher.should match_description("a ([#]) context with a spec")
      #   matcher.should match_description("a ([#]) context with a spec")
      # end
      
      # specify "should not match wrong spec only" do
      #   matcher=SpecMatcher.new("with another spec", "a context")
      #   matcher.should_not match_description("with a spec")
      # end
      # 
      # specify "should not match wrong context" do
      #   matcher=SpecMatcher.new("another context with a spec", "a context")
      #   matcher.should_not match_description("with a spec")
      # end
      # 
      # specify "should not match wrong context only" do
      #   matcher=SpecMatcher.new("another context", "a context")
      #   matcher.should_not match_description("with a spec")
      # end
      # 
      # specify "should not match wrong spec" do
      #   matcher=SpecMatcher.new("a context with another spec", "a context")
      #   matcher.should_not match_description("with a spec")
      # end
      
    end
  end
end