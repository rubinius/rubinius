require File.dirname(__FILE__) + '/../spec_helper'

describe "Calling a method" do
  it "just works" do
    def foo(a,b,c); [a,b,c] end

    foo(1,2,3).should == [1,2,3]
  end
  
  it "with no arguments is ok" do
    def mybar; 100 end

    mybar.should == 100
  end
  
  it "with block as block argument is ok" do
    def foo(a,&b); [a,yield(b)] end

    foo(10) do 200 end.should == [10,200]
    foo(10) { 200 }.should == [10,200]
  end

  it "with block argument converts the block to proc" do
    def makeproc(&b) b end
    makeproc { "hello" }.call.should == "hello"
    makeproc { "hello" }.class.should == Proc

    # check that converted proc is indeed behaves like proc,
    # not like lambda
    def proc_caller(&b) b.call end
    def enclosing_method
      proc_caller { return :break_return_value }
      :method_return_value
    end

    enclosing_method.should == :break_return_value
  end

  it "with lambda as block argument is ok" do
    def foo(a,&b); [a,yield(b)] end

    l = lambda { 300 }
    foo(10, &l).should == [10,300]
  end

  it "with an object that responds to 'to_proc' as a block argument coerces it to a proc" do
    x = "my proc"
    class << x
      def to_proc; Proc.new {|y| self + y}; end
    end

    def foo(&b); b.call(" called"); end
    def foo2; yield(" yielded"); end
    
    foo(&x).should == "my proc called"
    foo2(&x).should == "my proc yielded"
  end
  it "fails with both lambda and block argument" do
    def foo(a,&b); [a,yield(b)] end

    l = lambda { 300 }
    lambda { eval "foo(10, &l){ 42}"}.should raise_error(SyntaxError)
  end

  it "with same names as existing variables is ok" do
    foobar = 100

    def foobar; 200; end

    foobar.should == 100
    foobar().should == 200
  end

  it "with splat operator * and literal array unpacks params" do
    def foo(a,b,c); [a,b,c] end

    foo(*[1,2,3]).should == [1,2,3]
  end
  
  it "with splat operator * and referenced array unpacks params" do
    def foo(a,b,c); [a,b,c] end

    a = [1,2,3]
    foo(*a).should == [1,2,3]
  end

  it "without parentheses works" do
    def foo(a,b,c); [a,b,c] end

    (foo 1,2,3).should == [1,2,3]
  end
  
  it "with invalid argument count raises an ArgumentError" do
    def foo(a,b,c); end

    lambda { foo }.should raise_error(ArgumentError)
    lambda { foo(1,2) }.should raise_error(ArgumentError)
    lambda { foo(1,2,3,4) }.should raise_error(ArgumentError)
  end

  # "Allows infinite arguments" is kinda hard to spec
  it "allows any number of args beyond required to method with a splat" do
    def foo(a, b, *c); [c.size, c.last]; end

    a = Array.new(2500) { Object.new }
    obj = a[-1]

    lambda { foo 1 }.should raise_error(ArgumentError)

    res = foo 1, 2
    res.first.should == 0
    res.last.nil?.should == true

    res = foo 1, 2, 3
    res.first.should == 1
    res.last.should == 3

    res = foo 1, 2, 3, *a
    res.first.should == 2501
    res.last.should equal(obj)
  end
  
  it "allows to pass literal hashes without curly braces as the last parameter" do
    def foo(a,b,c); [a,b,c] end

    foo('abc', 456, 'rbx' => 'cool', 'specs' => 'fail sometimes', 'oh' => 'weh').should ==
      ['abc', 456, { 'rbx' => 'cool', 'specs' => 'fail sometimes', 'oh' => 'weh'}]
    
    (foo 'abc', 456, 'rbx' => 'cool', 'specs' => 'fail sometimes', 'oh' => 'weh').should ==
      ['abc', 456, { 'rbx' => 'cool', 'specs' => 'fail sometimes', 'oh' => 'weh'}]
  end
  
  it "allows to literal hashes without curly braces as the only parameter" do
    def foo(a); a end

    foo(:rbx => :cool, :specs => :fail_sometimes).should == 
      { :rbx => :cool, :specs => :fail_sometimes }
        
    (foo :rbx => :cool, :specs => :fail_sometimes).should == 
      { :rbx => :cool, :specs => :fail_sometimes }
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

  it "with range in () should give higher priority to range" do
    def myfoo(x); end

    def mybar(n)
      myfoo (0..n).map { }
    end

    mybar(10).should == nil
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
  
end

describe "Calling a private setter method" do
  it "permits self as a receiver" do
    class << self
      attr_reader :foo
      attr_writer :foo
      private :foo=
    end

    self.foo = 42
    self.foo.should == 42
  end
end

