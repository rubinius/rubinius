require File.dirname(__FILE__) + '/../spec_helper'
require File.dirname(__FILE__) + '/fixtures/yield'

describe "Assignment via yield" do

  it "assigns objects to block variables" do
    def f; yield nil; end;      f {|a| a.should == nil }
    def f; yield 1; end;        f {|a| a.should == 1 }
    def f; yield []; end;       f {|a| a.should == [] }
    def f; yield [1]; end;      f {|a| a.should == [1] }
    def f; yield [nil]; end;    f {|a| a.should == [nil] }
    def f; yield [[]]; end;     f {|a| a.should == [[]] }
    def f; yield [*[]]; end;    f {|a| a.should == [] }
    def f; yield [*[1]]; end;   f {|a| a.should == [1] }
    def f; yield [*[1,2]]; end; f {|a| a.should == [1,2] }
  end

  it "assigns splatted objects to block variables" do
    def f; yield *1; end;       f {|a| a.should == 1 }
  end

  it "assigns splatted objects to block variables" do
    def f; yield *nil; end;     f {|a| a.should == nil }
    def f; yield *[1]; end;     f {|a| a.should == 1 }
    def f; yield *[nil]; end;   f {|a| a.should == nil }
    def f; yield *[[]]; end;    f {|a| a.should == [] }
    def f; yield *[*[1]]; end;  f {|a| a.should == 1 }
  end

  it "assigns objects followed by splatted objects to block variables" do
    def f; yield 1, *nil; end;     f {|a, b| b.should == nil }
    def f; yield 1, *1; end;       f {|a, b| b.should == 1 }
    def f; yield 1, *[1]; end;     f {|a, b| b.should == 1 }
    def f; yield 1, *[nil]; end;   f {|a, b| b.should == nil }
    def f; yield 1, *[[]]; end;    f {|a, b| b.should == [] }
    def f; yield 1, *[*[1]]; end;  f {|a, b| b.should == 1 }
  end

  it "assigns objects to block variables that include the splat operator inside the block" do
    def f; yield; end;          f {|*a| a.should == [] }
    def f; yield nil; end;      f {|*a| a.should == [nil] }
    def f; yield 1; end;        f {|*a| a.should == [1] }
    def f; yield []; end;       f {|*a| a.should == [[]] }
    def f; yield [1]; end;      f {|*a| a.should == [[1]] }
    def f; yield [nil]; end;    f {|*a| a.should == [[nil]] }
    def f; yield [[]]; end;     f {|*a| a.should == [[[]]] }
    def f; yield [1,2]; end;    f {|*a| a.should == [[1,2]] }
    def f; yield [*[]]; end;    f {|*a| a.should == [[]] }
    def f; yield [*[1]]; end;   f {|*a| a.should == [[1]] }
    def f; yield [*[1,2]]; end; f {|*a| a.should == [[1,2]] }
  end

  it "assigns objects to multiple block variables" do
    def f; yield; end;          f {|a,b,*c| [a,b,c].should == [nil,nil,[]] }
    def f; yield nil; end;      f {|a,b,*c| [a,b,c].should == [nil,nil,[]] }
    def f; yield 1; end;        f {|a,b,*c| [a,b,c].should == [1,nil,[]] }
    def f; yield []; end;       f {|a,b,*c| [a,b,c].should == [nil,nil,[]] }
    def f; yield [1]; end;      f {|a,b,*c| [a,b,c].should == [1,nil,[]] }
    def f; yield [nil]; end;    f {|a,b,*c| [a,b,c].should == [nil,nil,[]] }
    def f; yield [[]]; end;     f {|a,b,*c| [a,b,c].should == [[],nil,[]] }
    def f; yield [*[]]; end;    f {|a,b,*c| [a,b,c].should == [nil,nil,[]] }
    def f; yield [*[1]]; end;   f {|a,b,*c| [a,b,c].should == [1,nil,[]] }
    def f; yield [*[1,2]]; end; f {|a,b,*c| [a,b,c].should == [1,2,[]] }
  end

  it "assigns objects to splatted block variables that include the splat operator inside the block" do
    def f; yield *1; end;        f {|*a| a.should == [1] }
    def f; yield *[]; end;       f {|*a| a.should == [] }
    def f; yield *[1]; end;      f {|*a| a.should == [1] }
    def f; yield *[nil]; end;    f {|*a| a.should == [nil] }
    def f; yield *[[]]; end;     f {|*a| a.should == [[]] }
    def f; yield *[*[]]; end;    f {|*a| a.should == [] }
    def f; yield *[*[1]]; end;   f {|*a| a.should == [1] }
    def f; yield *[*[1,2]]; end; f {|*a| a.should == [1,2] }
  end

  it "assigns splatted objects to multiple block variables" do
    def f; yield *nil; end;      f {|a,b,*c| [a,b,c].should == [nil,nil,[]] }
    def f; yield *1; end;        f {|a,b,*c| [a,b,c].should == [1,nil,[]] }
    def f; yield *[]; end;       f {|a,b,*c| [a,b,c].should == [nil,nil,[]] }
    def f; yield *[1]; end;      f {|a,b,*c| [a,b,c].should == [1,nil,[]] }
    def f; yield *[nil]; end;    f {|a,b,*c| [a,b,c].should == [nil,nil,[]] }
    def f; yield *[*[]]; end;    f {|a,b,*c| [a,b,c].should == [nil,nil,[]] }
    def f; yield *[*[1]]; end;   f {|a,b,*c| [a,b,c].should == [1,nil,[]] }
    def f; yield *[*[1,2]]; end; f {|a,b,*c| [a,b,c].should == [1,2,[]] }
  end

  ruby_version_is "" ... "1.9" do
    it "assigns objects to splatted block variables that include the splat operator inside the block" do
      def f; yield *nil; end;      f {|*a| a.should == [nil] }
    end

    it "assigns splatted objects to multiple block variables" do
      def f; yield *[[]]; end;     f {|a,b,*c| [a,b,c].should == [[],nil,[]] }
    end
  end
end

describe "The yield keyword" do
  it "raises a LocalJumpError when invoked in a method not passed a block" do
    lambda { YieldSpecs::no_block }.should raise_error(LocalJumpError)
  end
end
