require File.dirname(__FILE__) + '/../spec_helper'

describe "Assignment via return" do
  it "assigns objects to block variables" do
    def r; return nil; end;      a = r(); a.should == nil
    def r; return 1; end;        a = r(); a.should == 1
    def r; return []; end;       a = r(); a.should == []
    def r; return [1]; end;      a = r(); a.should == [1]
    def r; return [nil]; end;    a = r(); a.should == [nil]
    def r; return [[]]; end;     a = r(); a.should == [[]]
    def r; return [*[]]; end;    a = r(); a.should == []
    def r; return [*[1]]; end;   a = r(); a.should == [1]
    def r; return [*[1,2]]; end; a = r(); a.should == [1,2]

    # return with expressions separated by commas
    def r; return 1, 2, 3; end;
    a =  r(); a.should == [1, 2, 3]
    a = *r(); a.should == [1, 2, 3]
    *a =  r(); a.should == [[1, 2, 3]]
    *a = *r(); a.should == [1, 2, 3]
    a,*c = r(); [a, c].should == [1, [2,3]]
  end

  it "assigns splatted objects to block variables" do
    def r; return *nil; end;     a = r(); a.should == nil
    def r; return *1; end;       a = r(); a.should == 1
    def r; return *[]; end;      a = r(); a.should == nil
    def r; return *[1]; end;     a = r(); a.should == 1
    def r; return *[nil]; end;   a = r(); a.should == nil
    def r; return *[[]]; end;    a = r(); a.should == []
    def r; return *[*[1]]; end;  a = r(); a.should == 1
    def r; return *[*[1,2]]; end; a = r(); a.should == [1,2]
  end

  it "assigns objects to block variables that include the splat operator inside the block" do
    def r; return; end;          a = *r(); a.should == nil
    def r; return nil; end;      a = *r(); a.should == nil
    def r; return 1; end;        a = *r(); a.should == 1
    def r; return []; end;       a = *r(); a.should == nil
    def r; return [1]; end;      a = *r(); a.should == 1
    def r; return [nil]; end;    a = *r(); a.should == nil
    def r; return [[]]; end;     a = *r(); a.should == []
    def r; return [1,2]; end;    a = *r(); a.should == [1,2]
    def r; return [*[]]; end;    a = *r(); a.should == nil
    def r; return [*[1]]; end;   a = *r(); a.should == 1
    def r; return [*[1,2]]; end; a = *r(); a.should == [1,2]
  end
  
  it "assigns objects to splatted block variables that include the splat operator inside the block" do
    def r; return *nil; end;      *a = r(); a.should == [nil]
    def r; return *1; end;        *a = r(); a.should == [1]
    def r; return *[]; end;       *a = r(); a.should == [nil]
    def r; return *[1]; end;      *a = r(); a.should == [1]
    def r; return *[nil]; end;    *a = r(); a.should == [nil]
    def r; return *[[]]; end;     *a = r(); a.should == [[]]
    def r; return *[*[]]; end;    *a = r(); a.should == [nil]
    def r; return *[*[1]]; end;   *a = r(); a.should == [1]
    def r; return *[*[1,2]]; end; *a = r(); a.should == [[1,2]]
  end
  
  it "assigns objects to splatted block variables that include the splat operator inside the block" do
    def r; return *nil; end;      *a = *r(); a.should == [nil]
    def r; return *1; end;        *a = *r(); a.should == [1]
    def r; return *[]; end;       *a = *r(); a.should == [nil]
    def r; return *[1]; end;      *a = *r(); a.should == [1]
    def r; return *[nil]; end;    *a = *r(); a.should == [nil]
    def r; return *[[]]; end;     *a = *r(); a.should == []
    def r; return *[*[]]; end;    *a = *r(); a.should == [nil]
    def r; return *[*[1]]; end;   *a = *r(); a.should == [1]
    def r; return *[*[1,2]]; end; *a = *r(); a.should == [1,2]    
  end
  
  it "assigns objects to multiple block variables" do
    def r; return; end;          a,b,*c = r(); [a,b,c].should == [nil,nil,[]]
    def r; return nil; end;      a,b,*c = r(); [a,b,c].should == [nil,nil,[]]
    def r; return 1; end;        a,b,*c = r(); [a,b,c].should == [1,nil,[]]
    def r; return []; end;       a,b,*c = r(); [a,b,c].should == [nil,nil,[]]
    def r; return [1]; end;      a,b,*c = r(); [a,b,c].should == [1,nil,[]]
    def r; return [nil]; end;    a,b,*c = r(); [a,b,c].should == [nil,nil,[]]
    def r; return [[]]; end;     a,b,*c = r(); [a,b,c].should == [[],nil,[]]
    def r; return [*[]]; end;    a,b,*c = r(); [a,b,c].should == [nil,nil,[]]
    def r; return [*[1]]; end;   a,b,*c = r(); [a,b,c].should == [1,nil,[]]
    def r; return [*[1,2]]; end; a,b,*c = r(); [a,b,c].should == [1,2,[]]
  end
  
  it "assigns splatted objects to multiple block variables" do
    def r; return *nil; end;      a,b,*c = r(); [a,b,c].should == [nil,nil,[]]
    def r; return *1; end;        a,b,*c = r(); [a,b,c].should == [1,nil,[]]
    def r; return *[]; end;       a,b,*c = r(); [a,b,c].should == [nil,nil,[]]
    def r; return *[1]; end;      a,b,*c = r(); [a,b,c].should == [1,nil,[]]
    def r; return *[nil]; end;    a,b,*c = r(); [a,b,c].should == [nil,nil,[]]
    def r; return *[[]]; end;     a,b,*c = r(); [a,b,c].should == [nil,nil,[]]
    def r; return *[*[]]; end;    a,b,*c = r(); [a,b,c].should == [nil,nil,[]]
    def r; return *[*[1]]; end;   a,b,*c = r(); [a,b,c].should == [1,nil,[]]
    def r; return *[*[1,2]]; end; a,b,*c = r(); [a,b,c].should == [1,2,[]]    
  end
end

describe "Return from within a begin" do
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

describe "Executing return from within a block" do
  it "raises a LocalJumpError if there is no lexicaly enclosing method" do
    def f; yield end
    lambda { f { return 5 } }.should raise_error(LocalJumpError)
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

describe "The return statement" do
  it "raises a ThreadError if used to exit a thread" do
    lambda { Thread.new { return }.join }.should raise_error(ThreadError)
  end    
end

