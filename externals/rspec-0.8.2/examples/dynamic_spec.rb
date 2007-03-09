require File.dirname(__FILE__) + '/spec_helper'

context "The month march" do
  (1..10).each do |n|
    specify "The root of #{n} square should be #{n}" do
      Math.sqrt(n*n).should == n
    end
  end
end