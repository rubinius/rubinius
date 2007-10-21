require File.dirname(__FILE__) + '/../spec_helper'

describe "Basic assignment" do
  
  it "should allow the rhs to be assigned to the lhs" do
    a = nil;       a.should == nil
    a = 1;         a.should == 1
    a = [];        a.should == []
    a = [1];       a.should == [1]
    a = [nil];     a.should == [nil]
    a = [[]];      a.should == [[]]
    a = [1,2];     a.should == [1,2]
    a = [*[]];     a.should == []
    a = [*[1]];    a.should == [1]
    a = [*[1,2]];  a.should == [1, 2]
  end

  it "should allow the assignment of the rhs to the lhs using the rhs splat operator" do
    a = *nil;      a.should == nil
    a = *1;        a.should == 1
    a = *[];       a.should == nil
    a = *[1];      a.should == 1
    a = *[nil];    a.should == nil
    a = *[[]];     a.should == []
    a = *[1,2];    a.should == [1,2]
    a = *[*[]];    a.should == nil
    a = *[*[1]];   a.should == 1
    a = *[*[1,2]]; a.should == [1,2]
  end

  it "should allow the assignment of the rhs to the lhs using the lhs splat operator" do
    *a = nil;      a.should == [nil]
    *a = 1;        a.should == [1]
    *a = [];       a.should == [[]]
    *a = [1];      a.should == [[1]]
    *a = [nil];    a.should == [[nil]]
    *a = [[]];     a.should == [[[]]]
    *a = [1,2];    a.should == [[1,2]]
    *a = [*[]];    a.should == [[]]
    *a = [*[1]];   a.should == [[1]]
    *a = [*[1,2]]; a.should == [[1,2]]
  end
  
  it "should allow the assignment of rhs to the lhs using the lhs and rhs splat operators simultaneously" do
    *a = *nil;      a.should == [nil]
    *a = *1;        a.should == [1]
    *a = *[];       a.should == []
    *a = *[1];      a.should == [1]
    *a = *[nil];    a.should == [nil]
    *a = *[[]];     a.should == [[]]
    *a = *[1,2];    a.should == [1,2]
    *a = *[*[]];    a.should == []
    *a = *[*[1]];   a.should == [1]
    *a = *[*[1,2]]; a.should == [1,2]
  end

  it "should allow multiple values to be assigned" do
    a,b,*c = nil;       [a,b,c].should == [nil, nil, []]
    a,b,*c = 1;         [a,b,c].should == [1, nil, []]
    a,b,*c = [];        [a,b,c].should == [nil, nil, []]
    a,b,*c = [1];       [a,b,c].should == [1, nil, []]
    a,b,*c = [nil];     [a,b,c].should == [nil, nil, []]
    a,b,*c = [[]];      [a,b,c].should == [[], nil, []]
    a,b,*c = [1,2];     [a,b,c].should == [1,2,[]]
    a,b,*c = [*[]];     [a,b,c].should == [nil, nil, []]
    a,b,*c = [*[1]];    [a,b,c].should == [1, nil, []]
    a,b,*c = [*[1,2]];  [a,b,c].should == [1, 2, []]
    
    a,b,*c = *nil;      [a,b,c].should == [nil, nil, []]
    a,b,*c = *1;        [a,b,c].should == [1, nil, []]
    a,b,*c = *[];       [a,b,c].should == [nil, nil, []]
    a,b,*c = *[1];      [a,b,c].should == [1, nil, []]
    a,b,*c = *[nil];    [a,b,c].should == [nil, nil, []]
    a,b,*c = *[[]];     [a,b,c].should == [[], nil, []]
    a,b,*c = *[1,2];    [a,b,c].should == [1,2,[]]
    a,b,*c = *[*[]];    [a,b,c].should == [nil, nil, []]
    a,b,*c = *[*[1]];   [a,b,c].should == [1, nil, []]
    a,b,*c = *[*[1,2]]; [a,b,c].should == [1, 2, []]
  end
  
  it "should allow assignment through lambda" do
    f = lambda {|r,| r.should == []}
    f.call([], *[])

    f = lambda {|r,*l| r.should == []; l.should == [1]}
    f.call([], *[1])

    f = lambda{|x| x}
    f.call(42).should == 42
    f.call([42]).should == [42]
    f.call([[42]]).should == [[42]]
    f.call([42,55]).should == [42,55]

    f = lambda{|x,| x}
    f.call(42).should == 42
    f.call([42]).should == [42]
    f.call([[42]]).should == [[42]]
    f.call([42,55]).should == [42,55]

    f = lambda{|*x| x}
    f.call(42).should == [42]
    f.call([42]).should == [[42]]
    f.call([[42]]).should == [[[42]]]
    f.call([42,55]).should == [[42,55]]
    f.call(42,55).should == [42,55]
  end
  
  it 'should allow chained assignment' do
    (a = 1 + b = 2 + c = 4 + d = 8).should == 15
    d.should == 8
    c.should == 12
    b.should == 14
    a.should == 15
  end
end

describe "Assignment using expansion" do
  
  it "should succeed without conversion" do
    *x = (1..7).to_a
    x.should == [[1, 2, 3, 4, 5, 6, 7]]
  end
  
end

describe "Assigning multiple values" do
  
  it "should allow parallel assignment" do
    a, b = 1, 2
    a.should == 1
    b.should == 2

    a, = 1,2
    a.should == 1
  end
  
  it "should allow safe parallel swapping" do
    a, b = 1, 2
    a, b = b, a
    a.should == 2
    b.should == 1
  end

  it "should bundle remaining values to an array when using the splat operator" do
    a, *b = 1, 2, 3
    a.should == 1
    b.should == [2, 3]
    
    *a = 1, 2, 3
    a.should == [1, 2, 3]
    
    *a = 4
    a.should == [4]
    
    *a = nil
    a.should == [nil]
    
    a,=*[1]
    a.should == 1
    a,=*[[1]]
    a.should == [1]
    a,=*[[[1]]]
    a.should == [[1]]
  end
    
  it "should allow complex parallel assignment" do
    a, (b, c), d = 1, [2, 3], 4
    a.should == 1
    b.should == 2
    c.should == 3
    d.should == 4
    
    x, (y, z) = 1, 2, 3
    [x,y,z].should == [1,2,nil]
    x, (y, z) = 1, [2,3]
    [x,y,z].should == [1,2,3]
    x, (y, z) = 1, [2]
    [x,y,z].should == [1,2,nil]
  end
  
end

describe "Conditional assignment" do

  it "should assign the lhs if previously unassigned" do
    a=[]
    a[0] ||= "bar"
    a[0].should == "bar"

    h={}
    h["foo"] ||= "bar"
    h["foo"].should == "bar"

    h["foo".to_sym] ||= "bar"
    h["foo".to_sym].should == "bar"

    aa = 5
    aa ||= 25
    aa.should == 5

    bb ||= 25
    bb.should == 25

    cc &&=33
    cc.should == nil

    cc = 5
    cc &&=44
    cc.should == 44
  end
  
end

describe 'Single assignment' do
  it 'Assignment does not modify the lhs, it reassigns its reference' do
    a = 'Foobar'
    b = a
    b = 'Bazquux'
    a.should == 'Foobar'
    b.should == 'Bazquux'
  end

  it 'Assignment does not copy the object being assigned, just creates a new reference to it' do
    a = []
    b = a
    b << 1
    a.should == [1]
  end

  it 'If rhs has multiple arguments, lhs becomes an Array of them' do
    a = 1, 2, 3
    a.should == [1, 2, 3]
  end
end

describe 'Multiple assignment without grouping or splatting' do
  it 'An equal number of arguments on lhs and rhs assigns positionally' do
    a, b, c, d = 1, 2, 3, 4
    a.should == 1
    b.should == 2
    c.should == 3
    d.should == 4
  end 

  it 'If rhs has too few arguments, the missing ones on lhs are assigned nil' do
    a, b, c = 1, 2
    a.should == 1
    b.should == 2
    c.should == nil
  end

  it 'If rhs has too many arguments, the extra ones are silently not assigned anywhere' do
    a, b = 1, 2, 3
    a.should == 1
    b.should == 2
  end

  it 'The assignments are done in parallel so that lhs and rhs are independent of eachother without copying' do
    o_of_a, o_of_b = Object.new, Object.new
    a, b = o_of_a, o_of_b
    a, b = b, a
    a.equal?(o_of_b).should == true
    b.equal?(o_of_a).should == true
  end
end

describe 'Multiple assignments with splats' do
  # TODO make this normal once rubinius eval works
  compliant :mri do
    it '* on the lhs has to be applied to the last parameter' do
      should_raise(SyntaxError) { eval 'a, *b, c = 1, 2, 3' }
    end
  end

  it '* on the lhs collects all parameters from its position onwards as an Array or an empty Array' do
    a, *b = 1, 2
    c, *d = 1
    e, *f = 1, 2, 3
    g, *h = 1, [2, 3]
    *i = 1, [2,3]
    *j = [1,2,3]
    *k = 1,2,3

    a.should == 1
    b.should == [2]
    c.should == 1
    d.should == []
    e.should == 1
    f.should == [2, 3]
    g.should == 1
    h.should == [[2, 3]]
    i.should == [1, [2, 3]]
    j.should == [[1,2,3]]
    k.should == [1,2,3]
  end
end

describe 'Multiple assignments with grouping' do
  it 'A group on the lhs is considered one position and treats its corresponding rhs position like an Array' do
    a, (b, c), d = 1, 2, 3, 4
    e, (f, g), h = 1, [2, 3, 4], 5
    i, (j, k), l = 1, 2, 3
    a.should == 1
    b.should == 2
    c.should == nil
    d.should == 3
    e.should == 1
    f.should == 2
    g.should == 3
    h.should == 5
    i.should == 1
    j.should == 2
    k.should == nil
    l.should == 3
  end

  compliant :mri do
    it 'rhs cannot use parameter grouping, it is a syntax error' do
      should_raise(SyntaxError) { eval '(a, b) = (1, 2)' }
    end
  end
end

def reverse_foo(a,b);return b,a;end

describe "Multiple assignment" do
  it "should have the proper return value" do
    (a,b,*c = *[5,6,7,8,9,10]).should == [5,6,7,8,9,10]
    (d,e = reverse_foo(4,3)).should == [3,4]
    (f,g,h = reverse_foo(6,7)).should == [7,6]
    (i,*j = *[5,6,7]).should == [5,6,7]
    (k,*l = [5,6,7]).should == [5,6,7]
    a.should == 5
    b.should == 6
    c.should == [7,8,9,10]
    d.should == 3
    e.should == 4
    f.should == 7
    g.should == 6
    h.should == nil
    i.should == 5
    j.should == [6,7]
    k.should == 5
    l.should == [6,7]
  end
end

describe "Multiple assignment, array-style" do
  it "should have the proper return value" do
    (a,b = 5,6,7).should == [5,6,7]
    a.should == 5
    b.should == 6

    (c,d,*e = 99,8).should == [99,8]
    c.should == 99
    d.should == 8
    e.should == []

    (f,g,h = 99,8).should == [99,8]
    f.should == 99
    g.should == 8
    h.should == nil
  end
end

