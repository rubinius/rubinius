require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

describe "Enumerable#grep_v" do
  it "returns an array of all elements not equal to the pattern when called without a block" do
    pattern_cls = Class.new do
      def ===(obj)
        obj == '2'
      end
    end
    enum = EnumerableSpecs::Numerous.new('2', 'a', 'nil', '3', false)

    enum.grep_v(pattern_cls.new).should == ['a', 'nil', '3', false]
  end

  it "returns an array of all elements not equal to the pattern passed through the block" do
    pattern_cls = Class.new do
      def ===(obj)
        /^ca/ =~ obj
      end
    end
    enum = EnumerableSpecs::Numerous.new("cat", "coat", "car", "cadr", "cost")

    enum.grep_v(pattern_cls.new) { |i| i.upcase }.should == ["COAT", "COST"]
  end

  it "calls the block with an array when yielded with multiple arguments" do
    yields = []
    EnumerableSpecs::YieldsMixed.new.grep_v(nil) { |v| yields << v }

    yields.should == [1, [2], [3, 4], [5, 6, 7], [8, 9], []]
    EnumerableSpecs::YieldsMixed.new.grep_v(nil).should == yields
  end
end
