require File.dirname(__FILE__) + '/../spec_helper'

describe "Assignment via return" do
  
  it "should assign objects to block variables" do
    def r; return nil; end;      a = r(); a.should == nil
    def r; return 1; end;        a = r(); a.should == 1
    def r; return []; end;       a = r(); a.should == []
    def r; return [1]; end;      a = r(); a.should == [1]
    def r; return [nil]; end;    a = r(); a.should == [nil]
    def r; return [[]]; end;     a = r(); a.should == [[]]
    def r; return [*[]]; end;    a = r(); a.should == []
    def r; return [*[1]]; end;   a = r(); a.should == [1]
    def r; return [*[1,2]]; end; a = r(); a.should == [1,2]
  end
  
  it "should assign splatted objects to block variables" do
    def r; return *nil; end;     a = r(); a.should == nil
    def r; return *1; end;       a = r(); a.should == 1
    def r; return *[]; end;      a = r(); a.should == nil
    def r; return *[1]; end;     a = r(); a.should == 1
    def r; return *[nil]; end;   a = r(); a.should == nil
    def r; return *[[]]; end;    a = r(); a.should == []
    def r; return *[*[1]]; end;  a = r(); a.should == 1
    def r; return *[*[1,2]]; end; a = r(); a.should == [1,2]
  end

  it "should assign objects to block variables that include the splat operator inside the block" do
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
  
  it "should assign objects to splatted block variables that include the splat operator inside the block" do
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
  
  it "should assign objects to splatted block variables that include the splat operator inside the block" do
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
  
  it "should assign objects to multiple block variables" do
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
  
  it "should assign splatted objects to multiple block variables" do
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
