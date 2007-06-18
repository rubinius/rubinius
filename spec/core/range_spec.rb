require File.dirname(__FILE__) + '/../spec_helper'

# ==, ===, begin, each, end, eql?, exclude_end?, first, hash,
# include?, inspect, last, member?, step, to_s

describe "Range" do
  specify "== should return true if other has same begin, end, and exclude_end?" do
    ((0..2) == (0..2)).should == true
    ((5..10) == Range.new(5,10)).should == true
    ((1482..1911) == (1482...1911)).should == false
    (('G'..'M') == ('G'..'M')).should == true
    (('D'..'V') == Range.new('D','V')).should == true
    (('Q'..'X') == ('Q'...'X')).should == false
    ((0xffff..0xfffff) == (0xffff..0xfffff)).should == true
    ((0xffff..0xfffff) == Range.new(0xffff,0xfffff)).should == true
    ((0xffff..0xfffff) == (0xffff...0xfffff)).should == false
    ((0.5..2.4) == (0.5..2.4)).should == true
    ((0.5..2.4) == Range.new(0.5, 2.4)).should == true
    ((0.5..2.4) == (0.5...2.4)).should == false
  end

  specify "eql? should be a synonym for ==" do
    (0..2).eql?((0..2)).should == true
    (5..10).eql?(Range.new(5,10)).should == true
    (1482..1911).eql?((1482...1911)).should == false
    ('G'..'M').eql?('G'..'M').should == true
    ('D'..'V').eql?(Range.new('D','V')).should == true
    ('Q'..'X').eql?('Q'...'X').should == false
    (0xffff..0xfffff).eql?(0xffff..0xfffff).should == true
    (0xffff..0xfffff).eql?(Range.new(0xffff,0xfffff)).should == true
    (0xffff..0xfffff).eql?(0xffff...0xfffff).should == false
    (0.5..2.4).eql?(0.5..2.4).should == true
    (0.5..2.4).eql?(Range.new(0.5, 2.4)).should == true
    (0.5..2.4).eql?(0.5...2.4).should == false
  end

  specify "=== should return true if other is an element" do
    ((0..5) === 2).should == true
    ((-5..5) === 0).should == true
    ((-1...1) === 10.5).should == false
    ((-10..-2) === -2.5).should == true
    (('C'..'X') === 'M').should == true
    (('C'..'X') === 'A').should == false
    (('B'...'W') === 'W').should == false
    (('B'...'W') === 'Q').should == true
    ((0xffff..0xfffff) === 0xffffd).should == true
    ((0xffff..0xfffff) === 0xfffd).should == false
    ((0.5..2.4) === 2).should == true
    ((0.5..2.4) === 2.5).should == false
    ((0.5..2.4) === 2.4).should == true
    ((0.5...2.4) === 2.4).should == false
  end

  specify "include? and member? should be synonyms for ===" do
    (0..5).include?(2).should == true
    (-5..5).include?(0).should == true
    (-1...1).include?(10.5).should == false
    (-10..-2).include?(-2.5).should == true
    ('C'..'X').include?('M').should == true
    ('C'..'X').include?('A').should == false
    ('B'...'W').include?('W').should == false
    ('B'...'W').include?('Q').should == true
    (0xffff..0xfffff).include?(0xffffd).should == true
    (0xffff..0xfffff).include?(0xfffd).should == false
    (0.5..2.4).include?(2).should == true
    (0.5..2.4).include?(2.5).should == false
    (0.5..2.4).include?(2.4).should == true
    (0.5...2.4).include?(2.4).should == false

    (0..5).member?(2).should == true
    (-5..5).member?(0).should == true
    (-1...1).member?(10.5).should == false
    (-10..-2).member?(-2.5).should == true
    ('C'..'X').member?('M').should == true
    ('C'..'X').member?('A').should == false
    ('B'...'W').member?('W').should == false
    ('B'...'W').member?('Q').should == true
    (0xffff..0xfffff).member?(0xffffd).should == true
    (0xffff..0xfffff).member?(0xfffd).should == false
    (0.5..2.4).member?(2).should == true
    (0.5..2.4).member?(2.5).should == false
    (0.5..2.4).member?(2.4).should == true
    (0.5...2.4).member?(2.4).should == false
  end
  
  specify "begin should return the first element" do
    (-1..1).begin.should == -1
    (0..1).begin.should == 0
    (0xffff...0xfffff).begin.should == 65535
    ('Q'..'T').begin.should == 'Q'
    ('Q'...'T').begin.should == 'Q'
    (0.5..2.4).begin.should == 0.5
  end

  specify "first should be a synonym for begin" do
    (-1..1).first.should == -1
    (0..1).first.should == 0
    (0xffff...0xfffff).first.should == 65535
    ('Q'..'T').first.should == 'Q'
    ('Q'...'T').first.should == 'Q'
    (0.5..2.4).first.should == 0.5
  end
  
  specify "each should pass each element to the block" do
    a = []
    (-5..5).each { |i| a << i }
    a.should == [-5, -4, -3, -2, -1, 0, 1, 2, 3, 4, 5]

    a = []
    ('A'..'D').each { |i| a << i }
    a.should == ['A','B','C','D']

    a = []
    ('A'...'D').each { |i| a << i }
    a.should == ['A','B','C']
    
    a = []
    (0xfffd...0xffff).each { |i| a << i }
    a.should == [0xfffd,0xfffe]

    should_raise(TypeError) do
      (0.5..2.4).each
    end
  end

  specify "each should pass each element to the block" do
    (-5..5).to_a.should == [-5, -4, -3, -2, -1, 0, 1, 2, 3, 4, 5]
    ('A'..'D').to_a.should == ['A','B','C','D']
    ('A'...'D').to_a.should == ['A','B','C']    
    (0xfffd...0xffff).to_a.should == [0xfffd,0xfffe]
    should_raise(TypeError) do
      (0.5..2.4).to_a
    end
  end
  
  specify "end should return the last element" do
    (-1..1).end.should == 1
    (0..1).end.should == 1
    ("A".."Q").end.should == "Q"
    ("A"..."Q").end.should == "Q"
    (0xffff...0xfffff).end.should == 1048575
    (0.5..2.4).end.should == 2.4
  end

  specify "last should be a synonym for end" do
    (-1..1).last.should == 1
    (0..1).last.should == 1
    ("A".."Q").last.should == "Q"
    ("A"..."Q").last.should == "Q"
    (0xffff...0xfffff).last.should == 1048575
    (0.5..2.4).last.should == 2.4
  end
    
  specify "exclude_end? should return true if the range exludes the end value" do
    (-2..2).exclude_end?.should == false
    (0...5).exclude_end?.should == true
    ('A'..'B').exclude_end?.should == false
    ('A'...'B').exclude_end?.should == true
    (0xfffd..0xffff).exclude_end?.should == false
    (0xfffd...0xffff).exclude_end?.should == true
    (0.5..2.4).exclude_end?.should == false
    (0.5...2.4).exclude_end?.should == true
  end
  
  specify "should provide hash" do
    (0..1).respond_to?(:hash).should == true
    ('A'..'Z').respond_to?(:hash).should == true
    (0xfffd..0xffff).respond_to?(:hash).should == true
    (0.5..2.4).respond_to?(:hash).should == true
  end
  
  specify "inspect should provide a printable form" do
    (0..21).inspect.should == "0..21"
    (-8..0).inspect.should ==  "-8..0"
    (-411..959).inspect.should == "-411..959"
    ('A'..'Z').inspect.should == '"A".."Z"'
    ('A'...'Z').inspect.should == '"A"..."Z"'
    (0xfff..0xfffff).inspect.should == "4095..1048575"
    (0.5..2.4).inspect.should == "0.5..2.4"
  end

  specify "to_s should provide a printable form" do
    (0..21).to_s.should == "0..21"
    (-8..0).to_s.should ==  "-8..0"
    (-411..959).to_s.should == "-411..959"
    ('A'..'Z').to_s.should == 'A..Z'
    ('A'...'Z').to_s.should == 'A...Z'
    (0xfff..0xfffff).to_s.should == "4095..1048575"
    (0.5..2.4).inspect.should == "0.5..2.4"
  end

  specify "step should pass each nth element to the block" do
    a = []
    (-5..5).step(2) { |x| a << x }
    a.should == [-5, -3, -1, 1, 3, 5]

    a = []
    ("A".."F").step(2) { |x| a << x }
    a.should == ["A", "C", "E"]

    a = []
    ("A"..."G").step(2) { |x| a << x }
    a.should == ["A", "C", "E"]
    
    a = []
    (0.5..2.4).step(0.5) { |x| a << x}
    a.should == [0.5, 1, 1.5,2]
  end

  specify "step should not allow negative numbers in stepsize" do
    should_raise(ArgumentError) do
      a = []
      (-5..5).step(-2) { |x| a << x }
    end
  end

  specify "step should not allow zero in stepsize" do

    should_raise(ArgumentError) do
      a = []
      (-5..5).step(0) { |x| a << x }
    end

    should_raise(ArgumentError) do
      a = []
      (-5.5..5.7).step(0.0) { |x| a << x }
    end

  end

end
