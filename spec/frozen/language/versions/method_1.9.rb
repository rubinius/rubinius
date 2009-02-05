# FIXME: Add error case

describe "Calling a method" do
  it "works with required args after rest arguments" do
    def fooP0RQ1(*r, q); [r, q]; end
    fooP0RQ1(1).should == [[], 1]
    fooP0RQ1(1,2).should == [[1], 2]
    fooP0RQ1(1,2,3).should == [[1,2], 3]

     def fooP1RQ1(a, *r, q); [a, r, q]; end
     fooP1RQ1(1,2).should == [1, [], 2]
     fooP1RQ1(1,2,3).should == [1, [2], 3]
     fooP1RQ1(1,2,3,4).should == [1, [2, 3], 4]

     def fooP1O1RQ1(a, b=9, *r, q); [a, b, r, q]; end
     fooP1O1RQ1(1,2).should == [1, 9, [], 2]
     fooP1O1RQ1(1,2,3).should == [1, 2, [], 3]
     fooP1O1RQ1(1,2,3,4).should == [1, 2, [3], 4]
  end

  it "works with block arguments" do
    def fooP0Q0B(&a); [a.(1)]; end
    fooP0Q0B() { |z| z }.should == [1]
    def fooP1Q0B(a, &b); [a, b.(2)]; end
    fooP1Q0B(1) { |z| z }.should == [1, 2]
    def fooP1O1Q0B(a, b=2, &c); [a, b, c.(3)]; end
    fooP1O1Q0B(1) { |z| z }.should == [1, 2, 3]
    def fooP1O1RQ0B(a, b=2, *c, &d); [a, b, c, d.(5)]; end
    fooP1O1RQ0B(1, 2, 3, 4) { |z| z }.should == [1, 2, [3, 4], 5]
  end

  it "with an empty expression is like calling with nil argument" do
    def foo(a); a end
    foo(()).should be_nil
  end
  
#   it "with lambda as block argument is ok" do
#     def foo(a,&b); [a,yield(b)] end

#     l = lambda { 300 }
#     foo(10, &l).should == [10,300]
#   end

#   it "allows to pass argument, a hash without curly braces and a block argument" do
#     def foo(a,b,&c); [a,b,yield(c)] end

#     foo(:abc, 'rbx' => 'cool', 'specs' => 'fail sometimes') { 500 }.should ==
#       [:abc, { 'rbx' => 'cool', 'specs' => 'fail sometimes'}, 500]
    
#     foo(:abc, 'rbx' => 'cool', 'specs' => 'fail sometimes') do 500 end.should ==
#       [:abc, { 'rbx' => 'cool', 'specs' => 'fail sometimes'}, 500]

#     l = lambda { 500 }
    
#     foo(:abc, 'rbx' => 'cool', 'specs' => 'fail sometimes', &l).should ==
#       [:abc, { 'rbx' => 'cool', 'specs' => 'fail sometimes'}, 500]
#   end

  it "with range in () should give higher priority to range" do
    def myfoo(x); end

    def mybar(n)
      myfoo (0..n).map { }
    end

    mybar(10).should == nil
  end
  
#  it "with ambiguous missing parens, arguments go with innermost call" do
#    def f(*a); a.length; end
    
#    (f f 5, 6).should == 1

#    lambda { eval "f 4, f 5, 6" }.should raise_error(SyntaxError)

#    [f 5, 6].should == [2]
    
#    (f (5+6)*7).should == 1
#    (f(5+6)*7).should == 7
    
#    a, b = f 5, 6
#    a.should == 2
#    b.should == nil
#  end
  
end
