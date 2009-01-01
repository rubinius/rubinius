require File.dirname(__FILE__) + '/../../spec_helper'

extended_on :rubinius do
  describe "String#each_char" do
    it "passes each char in self to the given block" do
      a = []
      "hello".each_char { |c| a << c }
      a.should == ['h', 'e', 'l', 'l', 'o']
    end
  end
end
