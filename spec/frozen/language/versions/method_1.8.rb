describe "Calling a method" do
  it "with lambda as block argument is ok" do
    def foo(a,&b); [a,yield(b)] end

    l = lambda { 300 }
    foo(10, &l).should == [10,300]
  end

  it "allows to pass argument, a hash without curly braces and a block argument" do
    def foo(a,b,&c); [a,b,yield(c)] end

    foo(:abc, 'rbx' => 'cool', 'specs' => 'fail sometimes') { 500 }.should ==
      [:abc, { 'rbx' => 'cool', 'specs' => 'fail sometimes'}, 500]
    
    foo(:abc, 'rbx' => 'cool', 'specs' => 'fail sometimes') do 500 end.should ==
      [:abc, { 'rbx' => 'cool', 'specs' => 'fail sometimes'}, 500]

    l = lambda { 500 }
    
    foo(:abc, 'rbx' => 'cool', 'specs' => 'fail sometimes', &l).should ==
      [:abc, { 'rbx' => 'cool', 'specs' => 'fail sometimes'}, 500]
  end

  it "with ambiguous missing parens, arguments go with innermost call" do
    def f(*a); a.length; end
    
    (f f 5, 6).should == 1

    lambda { eval "f 4, f 5, 6" }.should raise_error(SyntaxError)

    [f 5, 6].should == [2]
    
    (f (5+6)*7).should == 1
    (f(5+6)*7).should == 7
    
    a, b = f 5, 6
    a.should == 2
    b.should == nil
  end

  it "with splat operator * and non-Array value attempts to coerce it to Array if the object respond_to?(:to_ary)" do
    def fooP3(a,b,c); a+b+c end
    def fooP4(a,b,c,d); a+b+c+d end

    obj = "pseudo-array"
    class << obj
      def to_ary; [2,3,4] end
    end
    fooP3(*obj).should == 9
    fooP4(1,*obj).should == 10
  end

  it "with splat operator * and non-Array value uses value unchanged if it does not respond_to?(:to_ary)" do
    def fooP0R(*args); args.length end

    obj = Object.new
    obj.should_not respond_to(:to_ary)
    fooP0R(*obj).should == 1
    fooP0R(1,2,3,*obj).should == 4
  end
end
