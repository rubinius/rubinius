require File.dirname(__FILE__) + '/../../../spec_helper.rb'

module Spec
  module Expectations
    module Helper
      context "should_not_be_arbitrary_predicate" do
        specify "should fail when method returns something other than true false or nil" do
          mock=HandCodedMock.new(5)
          lambda do
            mock.should_not_be_funny
          end.should_fail
          mock.__verify
        end

        specify "should fail when predicate accepts args and returns true" do
          mock=HandCodedMock.new(true)
          lambda do
            mock.should_not_be_hungry(1, 2, 3)
          end.should_fail
          mock.__verify
        end

        specify "should fail when predicate returns true" do
          mock=HandCodedMock.new(true)
          lambda do
            mock.should_not_be_funny
          end.should_fail
          mock.__verify
        end

        specify "should pass when predicate accepts args and returns false" do
          mock=HandCodedMock.new(false)
          lambda do
            mock.should_not_be_hungry(1, 2, 3)
          end.should_not_raise
          mock.__verify
        end

        specify "should pass when predicate returns false" do
          mock=HandCodedMock.new(false)
          mock.should_not_be_funny
          mock.__verify
        end

        specify "should pass when predicate returns nil" do
          mock=HandCodedMock.new(nil)
          mock.should_not_be_funny
          mock.__verify
        end

        specify "should raise NoMethodError when target does not respond to predicate" do
          lambda do
            5.should_not_be_funny
          end.should_raise(NameError)
        end

        specify "should support multi word predicates with should" do
          HandCodedMock.new(false).should_not_multi_word_predicate
        end

        specify "should support multi word predicates with should be" do
          HandCodedMock.new(false).should_not_be_multi_word_predicate
        end
      end
    end
  end
end
      