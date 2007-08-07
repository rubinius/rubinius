require File.dirname(__FILE__) + '/../spec_helper'

# ==, ===, begin, each, end, eql?, exclude_end?, first, hash,
# include?, inspect, last, member?, step, to_s

range_eql = shared "Range#eql?" do |cmd|
  describe "Range##{cmd}" do
    it "returns true if other has same begin, end, and exclude_end?" do
      (0..2).send(cmd, 0..2).should == true
      (5..10).send(cmd, Range.new(5,10)).should == true
      (1482..1911).send(cmd, 1482...1911).should == false
      ('G'..'M').send(cmd,'G'..'M').should == true
      ('D'..'V').send(cmd, Range.new('D','V')).should == true
      ('Q'..'X').send(cmd, 'Q'...'X').should == false
      (0xffff..0xfffff).send(cmd, 0xffff..0xfffff).should == true
      (0xffff..0xfffff).send(cmd, Range.new(0xffff,0xfffff)).should == true
      (0xffff..0xfffff).send(cmd, 0xffff...0xfffff).should == false
      (0.5..2.4).send(cmd, 0.5..2.4).should == true
      (0.5..2.4).send(cmd, Range.new(0.5, 2.4)).should == true
      (0.5..2.4).send(cmd, 0.5...2.4).should == false
    end
  end
end

describe "Range#==" do
  it_behaves_like(range_eql, :==)
end

describe "Range#eql?" do
  it_behaves_like(range_eql, :eql?)
end

range_include = shared "Range#include?" do |cmd|
  describe "Range##{cmd}" do
    it "returns true if other is an element" do
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
  end
end

describe "Range#===" do
  it_behaves_like(range_include, :===)
end

describe "Range#include?" do
  it_behaves_like(range_include, :include?)
end

describe "Range#member?" do
  it_behaves_like(range_include, :member?)
end

range_begin = shared "Range#begin" do |cmd|
  describe "Range##{cmd}" do
    it "returns the first element" do
      (-1..1).send(cmd).should == -1
      (0..1).send(cmd).should == 0
      (0xffff...0xfffff).send(cmd).should == 65535
      ('Q'..'T').send(cmd).should == 'Q'
      ('Q'...'T').send(cmd).should == 'Q'
      (0.5..2.4).send(cmd).should == 0.5
    end
  end
end

describe "Range#begin" do
  it_behaves_like(range_begin, :begin)
end

describe "Range#first" do
  it_behaves_like(range_begin, :first)
end

describe "Range#each" do
  it "passes each element to the block" do
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

  it "passes each element to the block" do
    (-5..5).to_a.should == [-5, -4, -3, -2, -1, 0, 1, 2, 3, 4, 5]
    ('A'..'D').to_a.should == ['A','B','C','D']
    ('A'...'D').to_a.should == ['A','B','C']    
    (0xfffd...0xffff).to_a.should == [0xfffd,0xfffe]
    should_raise(TypeError) do
      (0.5..2.4).to_a
    end
  end  
end

range_end = shared "Range#end" do |cmd|
  describe "Range##{cmd}" do
    it "end returns the last element" do
      (-1..1).send(cmd).should == 1
      (0..1).send(cmd).should == 1
      ("A".."Q").send(cmd).should == "Q"
      ("A"..."Q").send(cmd).should == "Q"
      (0xffff...0xfffff).send(cmd).should == 1048575
      (0.5..2.4).send(cmd).should == 2.4
    end
  end
end

describe "Range#end" do
  it_behaves_like(range_end, :end)
end

describe "Range#last" do
  it_behaves_like(range_end, :last)
end

describe "Range#exclude_end?" do
  it "returns true if the range exludes the end value" do
    (-2..2).exclude_end?.should == false
    (0...5).exclude_end?.should == true
    ('A'..'B').exclude_end?.should == false
    ('A'...'B').exclude_end?.should == true
    (0xfffd..0xffff).exclude_end?.should == false
    (0xfffd...0xffff).exclude_end?.should == true
    (0.5..2.4).exclude_end?.should == false
    (0.5...2.4).exclude_end?.should == true
  end  
end

describe "Range#hash" do
  it "is provided" do
    (0..1).respond_to?(:hash).should == true
    ('A'..'Z').respond_to?(:hash).should == true
    (0xfffd..0xffff).respond_to?(:hash).should == true
    (0.5..2.4).respond_to?(:hash).should == true
  end  
end

describe "Range#inspect" do
  it "provides a printable form" do
    (0..21).inspect.should == "0..21"
    (-8..0).inspect.should ==  "-8..0"
    (-411..959).inspect.should == "-411..959"
    ('A'..'Z').inspect.should == '"A".."Z"'
    ('A'...'Z').inspect.should == '"A"..."Z"'
    (0xfff..0xfffff).inspect.should == "4095..1048575"
    (0.5..2.4).inspect.should == "0.5..2.4"
  end
end

describe "Range#to_s" do
  it "provides a printable form" do
    (0..21).to_s.should == "0..21"
    (-8..0).to_s.should ==  "-8..0"
    (-411..959).to_s.should == "-411..959"
    ('A'..'Z').to_s.should == 'A..Z'
    ('A'...'Z').to_s.should == 'A...Z'
    (0xfff..0xfffff).to_s.should == "4095..1048575"
    (0.5..2.4).inspect.should == "0.5..2.4"
  end
end

describe "Range#step" do
  it "passes each nth element to the block" do
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

  it "does not allow negative numbers in stepsize" do
    should_raise(ArgumentError) do
      a = []
      (-5..5).step(-2) { |x| a << x }
    end
  end

  it "does not allow zero in stepsize" do
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
