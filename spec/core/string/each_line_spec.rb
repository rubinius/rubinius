require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes.rb'

describe "String#each_line" do
  it "is an alias of String#each" do
    [
      "", "x", "x\ny", "x\ry", "x\r\ny", "x\n\r\r\ny",
      "hello hullo bello", MyString.new("hello\nworld")
    ].each do |str|
      [
        [""], ["llo"], ["\n"], ["\r"], [nil],
        [], [MyString.new("\n")]
      ].each do |args|
        begin
          expected = []
          str.each(*args) { |x| expected << x << x.class }

          actual = []
          actual_cls = []
          r = str.each_line(*args) { |x| actual << x << x.class }
          r.equal?(str).should == true

          actual.should == expected
        end
      end
    end
  end
end
