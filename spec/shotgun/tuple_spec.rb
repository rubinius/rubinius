require File.dirname(__FILE__) + '/../spec_helper'

context "Tuple instance method" do
  specify "" do
    example do
      p Tuple.new(1)
    end.should == ''
  end
end
