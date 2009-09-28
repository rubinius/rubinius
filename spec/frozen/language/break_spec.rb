require File.dirname(__FILE__) + '/../spec_helper'

describe "The break statement" do
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

  it "causes block to return nil if an empty expression passed to break" do
    lambda { break (); 456 }.call.should be_nil
  end

  it "causes block to return nil if no value passed to break" do
    lambda { break; 456 }.call.should == nil
  end
end

describe "Executing break from within a block" do
  it "returns from the invoking singleton method" do
    obj = Object.new
    def obj.meth_with_block
      yield
      fail("break didn't break from the singleton method")
    end
    obj.meth_with_block { break :value }.should == :value
  end

  it "returns from the invoking method with the argument to break" do
    class BreakTest
      def self.meth_with_block
        yield
        fail("break didn't break from the method")
      end
    end
    BreakTest.meth_with_block { break :value }.should == :value
  end

  # Discovered in JRuby (see JRUBY-2756)
  it "returns from the original invoking method even in case of chained calls" do
    class BreakTest
      # case #1: yield
      def self.meth_with_yield(&b)
        yield
        fail("break returned from yield to wrong place")
      end
      def self.invoking_method(&b)
        meth_with_yield(&b)
        fail("break returned from 'meth_with_yield' method to wrong place")
      end

      # case #2: block.call
      def self.meth_with_block_call(&b)
        b.call
        fail("break returned from b.call to wrong place")
      end
      def self.invoking_method2(&b)
        meth_with_block_call(&b)
        fail("break returned from 'meth_with_block_call' method to wrong place")
      end
    end

    # this calls a method that calls another method that yields to the block
    BreakTest.invoking_method do
      break
      fail("break didn't, well, break")
    end

    # this calls a method that calls another method that calls the block
    BreakTest.invoking_method2 do
      break
      fail("break didn't, well, break")
    end

    res = BreakTest.invoking_method do
      break :return_value
      fail("break didn't, well, break")
    end
    res.should == :return_value

    res = BreakTest.invoking_method2 do
      break :return_value
      fail("break didn't, well, break")
    end
    res.should == :return_value

  end
end

describe "Breaking out of a loop with a value" do

  it "assigns objects" do
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

  ruby_version_is "" ... "1.9" do
    it "assigns splatted objects" do
      a = loop do break *nil; end;      a.should == nil
      a = loop do break *[]; end;       a.should == nil
      a = loop do break *1; end;        a.should == 1
      a = loop do break *[1]; end;      a.should == 1
      a = loop do break *[nil]; end;    a.should == nil
      a = loop do break *[[]]; end;     a.should == []
      a = loop do break *[*[]]; end;    a.should == nil
      a = loop do break *[1]; end;      a.should == 1
      a = loop do break *[*[1]]; end;   a.should == 1
      a = loop do break *[1,2]; end;    a.should == [1,2]
      a = loop do break *[*[1,2]]; end; a.should == [1,2]
    end
  end

  ruby_version_is "1.9" do
    it "assigns splatted objects" do
      a = loop do break *nil; end;      a.should == []
      a = loop do break *[]; end;       a.should == []
      a = loop do break *1; end;        a.should == [1]
      a = loop do break *[1]; end;      a.should == [1]
      a = loop do break *[nil]; end;    a.should == [nil]
      a = loop do break *[[]]; end;     a.should == [[]]
      a = loop do break *[*[]]; end;    a.should == []
      a = loop do break *[1]; end;      a.should == [1]
      a = loop do break *[*[1]]; end;   a.should == [1]
      a = loop do break *[1,2]; end;    a.should == [1,2]
      a = loop do break *[*[1,2]]; end; a.should == [1,2]
    end
  end

  ruby_version_is "" ... "1.9" do
    it "assigns to a splatted reference" do
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
  end

  ruby_version_is "1.9" do
    it "assigns to a splatted reference" do
      *a = loop do break; end;          a.should == [nil]
      *a = loop do break nil; end;      a.should == [nil]
      *a = loop do break 1; end;        a.should == [1]
      *a = loop do break []; end;       a.should == []
      *a = loop do break [1]; end;      a.should == [1]
      *a = loop do break [nil]; end;    a.should == [nil]
      *a = loop do break [[]]; end;     a.should == [[]]
      *a = loop do break [1,2]; end;    a.should == [1,2]
      *a = loop do break [*[]]; end;    a.should == []
      *a = loop do break [*[1]]; end;   a.should == [1]
      *a = loop do break [*[1,2]]; end; a.should == [1,2]
    end
  end

  ruby_version_is "" ... "1.9" do
    it "assigns splatted objects to a splatted reference" do
      *a = loop do break *nil; end;      a.should == [nil]
      *a = loop do break *[]; end;       a.should == [nil]
      *a = loop do break *[1,2]; end;    a.should == [[1,2]]
      *a = loop do break *[*[]]; end;    a.should == [nil]
      *a = loop do break *[*[1,2]]; end; a.should == [[1,2]]
      *a = loop do break *1; end;        a.should == [1]
      *a = loop do break *[1]; end;      a.should == [1]
      *a = loop do break *[nil]; end;    a.should == [nil]
      *a = loop do break *[[]]; end;     a.should == [[]]
      *a = loop do break *[*[1]]; end;   a.should == [1]
    end
  end

  ruby_version_is "1.9" do
    it "assigns splatted objects to a splatted reference" do
      *a = loop do break *nil; end;      a.should == []
      *a = loop do break *[]; end;       a.should == []
      *a = loop do break *[1,2]; end;    a.should == [1,2]
      *a = loop do break *[*[]]; end;    a.should == []
      *a = loop do break *[*[1,2]]; end; a.should == [1,2]
      *a = loop do break *1; end;        a.should == [1]
      *a = loop do break *[1]; end;      a.should == [1]
      *a = loop do break *[nil]; end;    a.should == [nil]
      *a = loop do break *[[]]; end;     a.should == [[]]
      *a = loop do break *[*[1]]; end;   a.should == [1]
    end
  end

  ruby_version_is "" ... "1.9" do
    it "assigns splatted objects to a splatted reference from a splatted loop" do
      *a = *loop do break *nil; end;      a.should == [nil]
      *a = *loop do break *[]; end;       a.should == [nil]
      *a = *loop do break *[[]]; end;     a.should == []
      *a = *loop do break *[*[]]; end;    a.should == [nil]
      *a = *loop do break *1; end;        a.should == [1]
      *a = *loop do break *[1]; end;      a.should == [1]
      *a = *loop do break *[nil]; end;    a.should == [nil]
      *a = *loop do break *[1,2]; end;    a.should == [1,2]
      *a = *loop do break *[*[1]]; end;   a.should == [1]
      *a = *loop do break *[*[1,2]]; end; a.should == [1,2]
    end
  end

  ruby_version_is "1.9" do
    it "assigns splatted objects to a splatted reference from a splatted loop" do
      *a = *loop do break *nil; end;      a.should == []
      *a = *loop do break *[]; end;       a.should == []
      *a = *loop do break *[[]]; end;     a.should == [[]]
      *a = *loop do break *[*[]]; end;    a.should == []
      *a = *loop do break *1; end;        a.should == [1]
      *a = *loop do break *[1]; end;      a.should == [1]
      *a = *loop do break *[nil]; end;    a.should == [nil]
      *a = *loop do break *[1,2]; end;    a.should == [1,2]
      *a = *loop do break *[*[1]]; end;   a.should == [1]
      *a = *loop do break *[*[1,2]]; end; a.should == [1,2]
    end
  end

  it "assigns objects to multiple block variables" do
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

  ruby_version_is "" ... "1.9" do
    it "assigns splatted objects to multiple block variables" do
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

  ruby_version_is "1.9" do
    it "assigns splatted objects to multiple block variables" do
      a,b,*c = loop do break *nil; end;      [a,b,c].should == [nil,nil,[]]
      a,b,*c = loop do break *1; end;        [a,b,c].should == [1,nil,[]]
      a,b,*c = loop do break *[]; end;       [a,b,c].should == [nil,nil,[]]
      a,b,*c = loop do break *[1]; end;      [a,b,c].should == [1,nil,[]]
      a,b,*c = loop do break *[nil]; end;    [a,b,c].should == [nil,nil,[]]
      a,b,*c = loop do break *[[]]; end;     [a,b,c].should == [[],nil,[]]
      a,b,*c = loop do break *[1,2]; end;    [a,b,c].should == [1,2,[]]
      a,b,*c = loop do break *[*[]]; end;    [a,b,c].should == [nil,nil,[]]
      a,b,*c = loop do break *[*[1]]; end;   [a,b,c].should == [1,nil,[]]
      a,b,*c = loop do break *[*[1,2]]; end; [a,b,c].should == [1,2,[]]
    end
  end

  ruby_version_is "" ... "1.9" do
    it "stops any loop type at the correct spot" do
      i = 0; loop do break i if i == 2; i+=1; end.should == 2
      i = 0; loop do break if i == 3; i+=1; end; i.should == 3
      i = 0; 0.upto(5) {|i| break i if i == 2 }.should == 2
      i = 0; 0.upto(5) {|i| break if i == 3 }; i.should == 3
      i = 0; while (i < 5) do break i if i == 2 ; i+=1; end.should == 2
      i = 0; while (i < 5) do break if i == 3 ; i+=1; end; i.should == 3
    end
  end

  ruby_version_is "1.9" do
    it "stops any loop type at the correct spot" do
      i = 0; loop do break i if i == 2; i+=1; end.should == 2
      i = 0; loop do break if i == 3; i+=1; end; i.should == 3
      i = 0; 0.upto(5) {|i| break i if i == 2 }.should == 2
      i = 0; 0.upto(5) {|i| break if i == 3 }; i.should == 0
      i = 0; while (i < 5) do break i if i == 2 ; i+=1; end.should == 2
      i = 0; while (i < 5) do break if i == 3 ; i+=1; end; i.should == 3
    end
  end

  ruby_version_is "" ... "1.9" do
    it "stops a yielded method at the correct spot" do
      def break_test()
        yield 1
        yield 2
        yield 3
      end
      break_test {|i| break i if i == 2 }.should == 2
      i = 0
      break_test {|i| break i if i == 1 }
      i.should == 1
    end
  end
  
  ruby_version_is "1.9" do
    it "stops a yielded method at the correct spot" do
      def break_test()
        yield 1
        yield 2
        yield 3
      end
      break_test {|i| break i if i == 2 }.should == 2
      i = 0
      break_test {|i| break i if i == 1 }
      i.should == 0
    end
  end
end

language_version __FILE__, "break"
