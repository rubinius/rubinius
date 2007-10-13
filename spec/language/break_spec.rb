require File.dirname(__FILE__) + '/../spec_helper'

describe "The break statement" do
  it "raises LocalJumpError if used not within block or while/for loop" do
    def x; break; end
    should_raise(LocalJumpError) { x }
  end

  it "ends block execution if used whithin block" do
    a = []
    lambda {
      a << 1
      break
      a << 2
    }.call
    a.should == [1]
  end

  it "causes block to return value passed to break" do
    lambda { break 123; 456 }.call.should == 123
  end

  it "causes block to return nil if no value passed to break" do
    lambda { break; 456 }.call.should == nil
  end
end

describe "Breaking out of a loop with a value" do

  it "should assign objects" do
    a = loop do break; end;          a.should == nil
    a = loop do break nil; end;      a.should == nil
    a = loop do break 1; end;        a.should == 1
    a = loop do break []; end;       a.should == []
    a = loop do break [1]; end;      a.should == [1]
    a = loop do break [nil]; end;    a.should == [nil]
    a = loop do break [[]]; end;     a.should == [[]]
    a = loop do break [*[]]; end;    a.should == []
    a = loop do break [*[1]]; end;   a.should == [1]
    a = loop do break [*[1,2]]; end; a.should == [1,2]
  end

  it "should assign splatted objects" do
    a = loop do break *nil; end;      a.should == nil
    a = loop do break *1; end;        a.should == 1
    a = loop do break *[]; end;       a.should == nil
    a = loop do break *[1]; end;      a.should == 1
    a = loop do break *[nil]; end;    a.should == nil
    a = loop do break *[[]]; end;     a.should == []
    a = loop do break *[*[]]; end;    a.should == nil
    a = loop do break *[1]; end;      a.should == 1
    a = loop do break *[*[1]]; end;   a.should == 1
    a = loop do break *[1,2]; end;    a.should == [1,2]
    a = loop do break *[*[1,2]]; end; a.should == [1,2]
  end

  it "should assign to a splatted reference" do
    *a = loop do break; end;          a.should == [nil]
    *a = loop do break nil; end;      a.should == [nil]
    *a = loop do break 1; end;        a.should == [1]
    *a = loop do break []; end;       a.should == [[]]
    *a = loop do break [1]; end;      a.should == [[1]]
    *a = loop do break [nil]; end;    a.should == [[nil]]
    *a = loop do break [[]]; end;     a.should == [[[]]]
    *a = loop do break [1,2]; end;    a.should == [[1,2]]
    *a = loop do break [*[]]; end;    a.should == [[]]
    *a = loop do break [*[1]]; end;   a.should == [[1]]
    *a = loop do break [*[1,2]]; end; a.should == [[1,2]]    
  end

  it "should assign splatted objects to a splatted reference" do
    *a = loop do break *nil; end;      a.should == [nil]
    *a = loop do break *1; end;        a.should == [1]
    *a = loop do break *[]; end;       a.should == [nil]
    *a = loop do break *[1]; end;      a.should == [1]
    *a = loop do break *[nil]; end;    a.should == [nil]
    *a = loop do break *[[]]; end;     a.should == [[]]
    *a = loop do break *[1,2]; end;    a.should == [[1,2]]
    *a = loop do break *[*[]]; end;    a.should == [nil]
    *a = loop do break *[*[1]]; end;   a.should == [1]
    *a = loop do break *[*[1,2]]; end; a.should == [[1,2]]    
  end

  it "should assign splatted objects to a splatted reference from a splatted loop" do
    *a = *loop do break *nil; end;      a.should == [nil]
    *a = *loop do break *1; end;        a.should == [1]
    *a = *loop do break *[]; end;       a.should == [nil]
    *a = *loop do break *[1]; end;      a.should == [1]
    *a = *loop do break *[nil]; end;    a.should == [nil]
    *a = *loop do break *[[]]; end;     a.should == []
    *a = *loop do break *[1,2]; end;    a.should == [1,2]
    *a = *loop do break *[*[]]; end;    a.should == [nil]
    *a = *loop do break *[*[1]]; end;   a.should == [1]
    *a = *loop do break *[*[1,2]]; end; a.should == [1,2]    
  end

  it "should assign objects to multiple block variables" do
    a,b,*c = loop do break; end;          [a,b,c].should == [nil,nil,[]]
    a,b,*c = loop do break nil; end;      [a,b,c].should == [nil,nil,[]]
    a,b,*c = loop do break 1; end;        [a,b,c].should == [1,nil,[]]
    a,b,*c = loop do break []; end;       [a,b,c].should == [nil,nil,[]]
    a,b,*c = loop do break [1]; end;      [a,b,c].should == [1,nil,[]]
    a,b,*c = loop do break [nil]; end;    [a,b,c].should == [nil,nil,[]]
    a,b,*c = loop do break [[]]; end;     [a,b,c].should == [[],nil,[]]
    a,b,*c = loop do break [1,2]; end;    [a,b,c].should == [1,2,[]]
    a,b,*c = loop do break [*[]]; end;    [a,b,c].should == [nil,nil,[]]
    a,b,*c = loop do break [*[1]]; end;   [a,b,c].should == [1,nil,[]]
    a,b,*c = loop do break [*[1,2]]; end; [a,b,c].should == [1,2,[]]
  end

  it "should assign splatted objects to multiple block variables" do
    a,b,*c = loop do break *nil; end;      [a,b,c].should == [nil,nil,[]]
    a,b,*c = loop do break *1; end;        [a,b,c].should == [1,nil,[]]
    a,b,*c = loop do break *[]; end;       [a,b,c].should == [nil,nil,[]]
    a,b,*c = loop do break *[1]; end;      [a,b,c].should == [1,nil,[]]
    a,b,*c = loop do break *[nil]; end;    [a,b,c].should == [nil,nil,[]]
    a,b,*c = loop do break *[[]]; end;     [a,b,c].should == [nil,nil,[]]
    a,b,*c = loop do break *[1,2]; end;    [a,b,c].should == [1,2,[]]
    a,b,*c = loop do break *[*[]]; end;    [a,b,c].should == [nil,nil,[]]
    a,b,*c = loop do break *[*[1]]; end;   [a,b,c].should == [1,nil,[]]
    a,b,*c = loop do break *[*[1,2]]; end; [a,b,c].should == [1,2,[]]    
  end
end
