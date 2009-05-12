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

  it "with splat operator * and non-Array value attempts to coerce it to Array if the object respond_to?(:to_a)" do
    def fooP3(a,b,c); a+b+c end
    def fooP4(a,b,c,d); a+b+c+d end

    obj = "pseudo-array"
    class << obj
      def to_a; [2,3,4] end
    end
    fooP3(*obj).should == 9
    fooP4(1,*obj).should == 10
  end

  it "with splat operator * and non-Array value uses value unchanged if it does not respond_to?(:to_a)" do
    def fooP0R(*args); args.length end

    obj = Object.new
    obj.should_not respond_to(:to_a)
    fooP0R(*obj).should == 1
    fooP0R(1,2,*obj,3).should == 4
  end

  it "accepts additional arguments after splat expansion" do
    def fooP4(a,b,c,d); a+b+c+d end

    a = [1,2]
    fooP4(*a,3,4).should == 10
    fooP4(0,*a,3).should == 6
  end

  it "accepts multiple splat expansions in the same argument list" do
    def fooP0R(*args); args.length end

    a = [1,2,3]
    b = 7
    c = "pseudo-array"
    def c.to_a; [0,0] end
    fooP0R(*a,*[4,5],6,*b).should == 7
    fooP0R(*a,*a,*a).should == 9
    fooP0R(0,*a,4,*5,6,7,*c,-1).should == 11
  end

  it "expands an array to arguments grouped in parentheses" do
    def fooP1((a,b)); a+b; end

    fooP1([40,2]).should == 42
  end

  it "expands an array to arguments grouped in parentheses and ignores any rest arguments in the array" do
    def fooP1((a,b)); a+b; end

    fooP1([40,2,84]).should == 42
  end

  it "expands an array to arguments grouped in parentheses and sets not specified arguments to nil" do
    def fooP1((a,b)); [a,b]; end

    fooP1([42]).should == [42, nil]
  end

  it "expands an array to arguments grouped in parentheses which in turn takes rest arguments" do
    def fooP1((a,b,*c,d,e)); [a,b,c,d,e]; end

    fooP1([1, 2, 3]).should == [1, 2, [], 3, nil]
    fooP1([1, 2, 3, 4]).should == [1, 2, [], 3, 4]
    fooP1([1, 2, 3, 4, 5]).should == [1, 2, [3], 4, 5]
  end
end
