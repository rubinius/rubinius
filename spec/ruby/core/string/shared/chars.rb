# -*- encoding: utf-8 -*-
require File.expand_path('../../../../spec_helper', __FILE__)
require File.expand_path('../../fixtures/classes', __FILE__)

describe :string_chars, :shared => true do
  it "passes each char in self to the given block" do
    a = []
    "hello".send(@method) { |c| a << c }
    a.should == ['h', 'e', 'l', 'l', 'o']
  end

  ruby_bug 'redmine #1487', '1.9.1' do
    it "returns self" do
      s = StringSpecs::MyString.new "hello"
      s.send(@method){}.should equal(s)
    end
  end


  it "is unicode aware" do
    before = $KCODE
    $KCODE = "UTF-8"
    "\303\207\342\210\202\303\251\306\222g".send(@method).to_a.should == ["\303\207", "\342\210\202", "\303\251", "\306\222", "g"]
    $KCODE = before
  end
end
