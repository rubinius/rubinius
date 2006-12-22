require 'rubygems'
require 'spec'
require File.dirname(__FILE__) + '/../mri_target'

context "MRITarget" do
  setup do
    @target = MRITarget.new
  end
  
  specify "should provide example method" do
    @target.should_respond_to :example
  end
  
  specify "example should return the value of the block it receives" do
    a = [1, 2, 3]
    @target.example { a }.should_equal? a
  end
end
