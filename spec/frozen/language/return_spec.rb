require File.dirname(__FILE__) + '/../spec_helper'

describe "The return keyword" do
  it "returns any object directly" do
    def r; return 1; end
    r().should == 1
  end

  it "returns an single element array directly" do
    def r; return [1]; end
    r().should == [1]
  end

  it "returns an multi element array directly" do
    def r; return [1,2]; end
    r().should == [1,2]
  end

  it "returns nil by default" do
    def r; return; end
    r().should be_nil
  end


  describe "in a Thread" do
    it "raises a ThreadError if used to exit a thread" do
      lambda { Thread.new { return }.join }.should raise_error(ThreadError)
    end
  end


  describe "when passed a splat" do
    it "returns nil when the ary is empty" do
      def r; ary = []; return *ary; end
      r.should be_nil
    end

    it "returns the first element when the array is size of 1" do
      def r; ary = [1]; return *ary; end
      r.should == 1
    end

    it "returns the whole array when size is greater than 1" do
      def r; ary = [1,2]; return *ary; end
      r.should == [1,2]

      def r; ary = [1,2,3]; return *ary; end
      r.should == [1,2,3]
    end

    it "returns a non-array when used as a splat" do
      def r; value = 1; return *value; end
      r.should == 1
    end

    it "calls 'to_a' on the splatted value first" do
      def r
        obj = Object.new
        def obj.to_a
          []
        end

        return *obj
      end

      r().should be_nil

      def r
        obj = Object.new
        def obj.to_a
          [1]
        end

        return *obj
      end

      r().should == 1

      def r
        obj = Object.new
        def obj.to_a
          [1,2]
        end

        return *obj
      end

      r().should == [1,2]
    end

    it "calls 'to_ary' on the splatted value first" do
      def r
        obj = Object.new
        def obj.to_ary
          []
        end

        return *obj
      end

      r().should be_nil

      def r
        obj = Object.new
        def obj.to_ary
          [1]
        end

        return *obj
      end

      r().should == 1

      def r
        obj = Object.new
        def obj.to_ary
          [1,2]
        end

        return *obj
      end

      r().should == [1,2]
    end
  end


  describe "within a begin" do
    it "executes ensure before returning from function" do
      def f(a)
        begin
          return a
        ensure
          a << 1
        end
      end
      f([]).should == [1]
    end

    it "executes return in ensure before returning from function" do
      def f(a)
        begin
          return a
        ensure
          return [0]
          a << 1
        end
      end
      f([]).should == [0]
    end

    it "executes ensures in stack order before returning from function" do
      def f(a)
        begin
          begin
            return a
          ensure
            a << 2
          end
        ensure
          a << 1
        end
      end
      f([]).should == [2,1]
    end

    it "executes return at base of ensure stack" do
      def f(a)
        begin
          begin
            return a
          ensure
            a << 2
            return 2
          end
        ensure
          a << 1
          return 1
        end
      end
      a = []
      f(a).should == 1
      a.should == [2, 1]
    end
  end


  describe "within a block" do
    it "raises a LocalJumpError if there is no lexicaly enclosing method" do
      def f; yield; end
      lambda { f { return 5 } }.should raise_error(LocalJumpError)
    end

    it "causes lambda to return nil if invoked without any arguments" do
      lambda { return; 456 }.call.should be_nil
    end

    it "causes lambda to return nil if invoked with an empty expression" do
      lambda { return (); 456 }.call.should be_nil
    end

    it "causes lambda to return the value passed to return" do
      lambda { return 123; 456 }.call.should == 123
    end

    it "causes the method that lexically encloses the block to return" do
      def meth_with_yield
        yield
        fail("return returned to wrong location")
      end

      def enclosing_method
        meth_with_yield do
          return :return_value
          fail("return didn't, well, return")
        end
        fail("return should not behave like break")
      end

      enclosing_method.should == :return_value
    end

    it "returns from the lexically enclosing method even in case of chained calls" do
      class ChainedReturnTest
        def self.meth_with_yield(&b)
          yield
          fail("returned from yield to wrong place")
        end
        def self.invoking_method(&b)
          meth_with_yield(&b)
          fail("returned from 'meth_with_yield' method to wrong place")
        end
        def self.enclosing_method
          invoking_method do
            return :return_value
            fail("return didn't, well, return")
          end
          fail("return should not behave like break")
        end
      end

      ChainedReturnTest.enclosing_method.should == :return_value
    end
  end
end
