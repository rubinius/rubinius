require File.expand_path('../../spec_helper', __FILE__)

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

  before :each do
    ScratchPad.clear
  end

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

  class BreakTest2
    def one
      two { yield }
    end

    def two
      yield
    ensure
      ScratchPad << :two_ensure
    end

    def three
      begin
        one { break }
        ScratchPad << :three_post
      ensure
        ScratchPad << :three_ensure
      end
    end
  end

  it "runs ensures when continuing upward" do
    ScratchPad.record []

    bt2 = BreakTest2.new
    bt2.one { break }
    ScratchPad.recorded.should == [:two_ensure]
  end

  it "doesn't run ensures in the destination method" do
    ScratchPad.record []

    bt2 = BreakTest2.new
    bt2.three
    ScratchPad.recorded.should == [:two_ensure, :three_post, :three_ensure]
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

  it "assigns splatted objects" do
    a = loop do break *[1,2]; end;    a.should == [1,2]
    a = loop do break *[*[1,2]]; end; a.should == [1,2]
  end

  ruby_version_is "" ... "1.9" do
    it "assigns nil objects when the splatted object is nil, an empty array or contains a nil object" do
      a = loop do break *nil; end;      a.should == nil
      a = loop do break *[]; end;       a.should == nil
      a = loop do break *[nil]; end;    a.should == nil
      a = loop do break *[*[]]; end;    a.should == nil
    end

    it "assigns an empty array when the splatted array contains an empty array" do
      a = loop do break *[[]]; end;     a.should == []
    end

    it "assigns the splatted object if it doesn't respont to to_ary" do
      a = loop do break *1; end;        a.should == 1
      a = loop do break *[1]; end;      a.should == 1
      a = loop do break *[*[1]]; end;   a.should == 1
    end
  end

  ruby_version_is "1.9" do
    it "assigns splatted objects" do
      a = loop do break *nil; end;      a.should == []
      a = loop do break *[]; end;       a.should == []
      a = loop do break *[*[]]; end;    a.should == []
    end

    it "assigns an array containing an empty array when the splatted array contains an empty array" do
      a = loop do break *[[]]; end;     a.should == [[]]
    end

    it "assigns an array with a nil object when the splatted object contains a nil object" do
      a = loop do break *[nil]; end;    a.should == [nil]
    end

    it "assigns an array that contains the splatted object if the splatted objects doesn't respond to to_a" do
      a = loop do break *1; end;        a.should == [1]
      a = loop do break *[1]; end;      a.should == [1]
      a = loop do break *[*[1]]; end;   a.should == [1]
    end
  end

  it "assigns to a splatted reference" do
    *a = loop do break; end;          a.should == [nil]
    *a = loop do break nil; end;      a.should == [nil]
    *a = loop do break 1; end;        a.should == [1]
  end

  ruby_version_is "" ... "1.9" do
    it "assigns arrays into another array to a splatted reference" do
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
    it "assigns arrays to a splatted reference" do
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

  it "assings splatted objects to a splatted reference" do
    *a = loop do break *1; end;        a.should == [1]
    *a = loop do break *[1]; end;      a.should == [1]
    *a = loop do break *[nil]; end;    a.should == [nil]
    *a = loop do break *[[]]; end;     a.should == [[]]
    *a = loop do break *[*[1]]; end;   a.should == [1]
  end

  ruby_version_is "" ... "1.9" do
    it "assigns arrays with nil objects to a splatted reference" do
      *a = loop do break *nil; end;      a.should == [nil]
      *a = loop do break *[]; end;       a.should == [nil]
      *a = loop do break *[*[]]; end;    a.should == [nil]
    end

    it "assigns an array containing an array to splatted reference when the object is an splatted array" do
      *a = loop do break *[1,2]; end;    a.should == [[1,2]]
      *a = loop do break *[*[1,2]]; end; a.should == [[1,2]]
    end
  end

  ruby_version_is "1.9" do
    it "assigns empty arrays to a splatted reference" do
      *a = loop do break *nil; end;      a.should == []
      *a = loop do break *[]; end;       a.should == []
      *a = loop do break *[*[]]; end;    a.should == []
    end

    it "assigns an array to splatted reference when the object is an splatted array" do
      *a = loop do break *[1,2]; end;    a.should == [1,2]
      *a = loop do break *[*[1,2]]; end; a.should == [1,2]
    end
  end

  it "assigns splatted objects to a splatted reference from a splatted loop" do
    *a = *loop do break *1; end;        a.should == [1]
    *a = *loop do break *[1]; end;      a.should == [1]
    *a = *loop do break *[nil]; end;    a.should == [nil]
    *a = *loop do break *[1,2]; end;    a.should == [1,2]
    *a = *loop do break *[*[1]]; end;   a.should == [1]
    *a = *loop do break *[*[1,2]]; end; a.should == [1,2]
  end

  ruby_version_is "" ... "1.9" do
    it "assigns arrays with a nil object to a splatted reference from a splatted loop" do
      *a = *loop do break *nil; end;      a.should == [nil]
      *a = *loop do break *[]; end;       a.should == [nil]
      *a = *loop do break *[*[]]; end;    a.should == [nil]
    end

    it "assigns an empty array to a splatted reference when the splatted array from a splatted loop contains an empty array" do
      *a = *loop do break *[[]]; end;     a.should == []
    end
  end

  ruby_version_is "1.9" do
    it "assigns empty arrays to a splatted reference from a splatted loop" do
      *a = *loop do break *nil; end;      a.should == []
      *a = *loop do break *[]; end;       a.should == []
      *a = *loop do break *[*[]]; end;    a.should == []
    end

    it "assigns an array containing an empty array to a splatter reference when the splatter array from a splatted loop contains an empty array" do
      *a = *loop do break *[[]]; end;     a.should == [[]]
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

  it "assigns splatted objects to multiple block variables" do
    a,b,*c = loop do break *nil; end;      [a,b,c].should == [nil,nil,[]]
    a,b,*c = loop do break *1; end;        [a,b,c].should == [1,nil,[]]
    a,b,*c = loop do break *[]; end;       [a,b,c].should == [nil,nil,[]]
    a,b,*c = loop do break *[1]; end;      [a,b,c].should == [1,nil,[]]
    a,b,*c = loop do break *[nil]; end;    [a,b,c].should == [nil,nil,[]]
    a,b,*c = loop do break *[1,2]; end;    [a,b,c].should == [1,2,[]]
    a,b,*c = loop do break *[*[]]; end;    [a,b,c].should == [nil,nil,[]]
    a,b,*c = loop do break *[*[1]]; end;   [a,b,c].should == [1,nil,[]]
    a,b,*c = loop do break *[*[1,2]]; end; [a,b,c].should == [1,2,[]]
  end

  ruby_version_is "" ... "1.9" do
    it "assigns nil to variables when the splatted value is an empty array" do
      a,b,*c = loop do break *[[]]; end;     [a,b,c].should == [nil,nil,[]]
    end
  end

  ruby_version_is "1.9" do
    it "assigns an empty array to variables when the splatted value is an empty array" do
      a,b,*c = loop do break *[[]]; end;     [a,b,c].should == [[],nil,[]]
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
