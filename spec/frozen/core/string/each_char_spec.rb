# encoding: utf-8
require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

ruby_version_is '1.8.7' do
  describe "String#each_char" do
    it "passes each char in self to the given block" do
      a = []
      "hello".each_char { |c| a << c }
      a.should == ['h', 'e', 'l', 'l', 'o']
    end

    ruby_bug 'redmine #1487', '1.9.1' do
      it "returns self" do
        s = StringSpecs::MyString.new "hello"
        s.each_char{}.should equal(s)
      end
    end

    it "returns an enumerator when no block given" do
      enum = "hello".each_char
      enum.should be_kind_of(enumerator_class)
      enum.to_a.should == ['h', 'e', 'l', 'l', 'o']
    end

    it "is unicode aware" do
      before = $KCODE
      $KCODE = "UTF-8"
      "\303\207\342\210\202\303\251\306\222g".each_char.to_a.should == ["\303\207", "\342\210\202", "\303\251", "\306\222", "g"]
      $KCODE = before
    end

  end
end
