require File.dirname(__FILE__) + '/../spec_helper'

# ==, ===, begin, each, end, eql?, exclude_end?, first, hash,
# include?, inspect, last, member?, step, to_s

context "Range" do
  specify "== should return true if other has same begin, end, and exclude_end?" do
    example do
      p [(0..2) == (0..2), (5..10) == Range.new(5,10), (1482..1911) == (1482...1911)]
    end.should == '[true, true, false]'
  end

  specify "=== should return true if other is an element" do
    example do
      p [(0..5) === 2, (-5..5) === 0, (-1...1) === 10.5, (-10..-2) === -2.5]
    end.should == '[true, true, false, true]'
  end
  
  specify "begin should return the first element" do
    example do
      p [(-1..1).begin, (0..1).begin, (0xffff...0xfffff).begin]
    end.should == '[-1, 0, 65535]'
  end
  
  specify "each should pass each element to the block" do
    example do
      (-5..5).each { |i| puts i }
    end.should == "-5\n-4\n-3\n-2\n-1\n0\n1\n2\n3\n4\n5"
  end
  
  specify "end should return the last element" do
    example do
      p [(-1..1).end, (0..1).end, (0xffff...0xfffff).end]
    end.should == '[1, 1, 1048575]'
  end
  
  specify "eql? should be a synonym for ==" do
    example do
      p [(0..2).eql?((0..2)), (5..10).eql?(Range.new(5,10)), (1482..1911).eql?((1482...1911))]
    end.should == '[true, true, false]'
  end
  
  specify "exclude_end? should return true if the range exludes the end value" do
    example do
      p [(-2..2).exclude_end?, (0...5).exclude_end?]
    end.should == '[false, true]'
  end
  
  specify "first should be a synonym for begin" do
    example do
      p [(-1..1).first, (0..1).first, (0xffff...0xfffff).first]
    end.should == '[-1, 0, 65535]'
  end
  
  specify "should provide hash" do
    example do
      p((0..1).respond_to?(:hash))
    end.should == 'true'
  end
  
  specify "include? should be a synonym for ===" do
    example do
      p [(0..5) === 2, (-5..5) === 0, (-1...1) === 10.5, (-10..-2) === -2.5]
    end.should == '[true, true, false, true]'
  end
  
  specify "inspect should provide a printable form" do
    example do
      p [(0..21).inspect, (-8..0).inspect, (-411..959).inspect]
    end.should == '["0..21", "-8..0", "-411..959"]'
  end

  specify "last should be a synonym for end" do
    example do
      p [(-1..1).end, (0..1).end, (0xffff...0xfffff).end]
    end.should == '[1, 1, 1048575]'
  end

  specify "member? should be a synonym for ===" do
    example do
      p [(0..5) === 2, (-5..5) === 0, (-1...1) === 10.5, (-10..-2) === -2.5]
    end.should == '[true, true, false, true]'
  end

  specify "step should pass each nth element to the block" do
    example do
      (-5..5).step(2) { |x| puts x }
    end.should == "-5\n-3\n-1\n1\n3\n5"
  end

  specify "to_s should provide a printable form" do
    example do
      p [(0..21).to_s, (-8..0).to_s, (-411..959).to_s]
    end.should == '["0..21", "-8..0", "-411..959"]'
  end
end
