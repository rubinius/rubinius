require File.dirname(__FILE__) + '/../../../spec_helper.rb'

module Spec
  module Expectations
    module Helper
      context "should_be_<arbitrary predicate>" do
        specify "should fail when method returns something other than true false or nil" do
          mock=HandCodedMock.new(5)
          lambda do
            mock.should_be_funny
          end.should_pass
          mock.__verify
        end
    
        specify "should fail when predicate accepts args and returns false" do
          mock=HandCodedMock.new(false)
          lambda do
            mock.should_be_hungry(1, 2, 3)
          end.should_fail_with "expected hungry?(1, 2, 3) to return true, got false"
          mock.__verify
        end
    
        specify "should fail when predicate returns false" do
          mock=HandCodedMock.new(false)
          lambda do
            mock.should_be_funny
          end.should_fail
        end
    
        specify "should fail when predicate returns nil" do
          mock=HandCodedMock.new(nil)
          lambda do
            mock.should_be_funny
          end.should_fail
          mock.__verify
        end
    
        specify "should pass when predicate accepts args and returns true" do
          mock=HandCodedMock.new(true)
          lambda do
            mock.should_be_hungry(1, 2, 3)
          end.should_pass
          mock.__verify
        end
    
        specify "should pass when predicate returns true" do
          mock=HandCodedMock.new(true)
          lambda do
            mock.should_be_funny
          end.should_pass
          mock.__verify
        end

        specify "should raise when target does not respond to predicate" do
          lambda do
            5.should_be_funny
          end.should_raise(NameError)
        end

        specify "should support multi word predicates with should" do
          lambda do
            HandCodedMock.new(true).should_multi_word_predicate
          end.should_pass
        end
    
        specify "should support multi word predicates with should be" do
          lambda do
            HandCodedMock.new(true).should_be_multi_word_predicate
          end.should_pass
        end
    
        specify "should support present tense (i.e. t.should_exist evaluates t.exists?)" do
          mock=HandCodedMock.new(true)
          lambda do
            mock.should_exist
          end.should_pass
        end
      end
    end
  end
end