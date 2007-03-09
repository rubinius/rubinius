require File.dirname(__FILE__) + '/../../../spec_helper.rb'

module Spec
  module Expectations
    module Helper
      context "should_have_at_most" do
        setup do
          @owner = CollectionOwner.new
          (1..3).each do |n|
            @owner.add_to_collection_with_length_method(n)
            @owner.add_to_collection_with_size_method(n)
          end
        end

        specify "should fail when expecting actual length" do
          @owner.should_have_at_most(3).items_in_collection_with_length_method
        end

        specify "should fail when expecting actual size" do
          @owner.should_have_at_most(3).items_in_collection_with_size_method
        end

        specify "should fail when expecting more than actual length" do
          @owner.should_have_at_most(4).items_in_collection_with_length_method
        end

        specify "should fail when expecting more than actual size" do
          @owner.should_have_at_most(4).items_in_collection_with_size_method
        end

        specify "should pass when expecting less than actual length" do
          lambda do
            @owner.should_have_at_most(2).items_in_collection_with_length_method
          end.should_fail
        end

        specify "should pass when expecting less than actual size" do
          lambda do
            @owner.should_have_at_most(2).items_in_collection_with_size_method
          end.should_fail
        end
      end
    end
  end
end
