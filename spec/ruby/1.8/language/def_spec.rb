require File.dirname(__FILE__) + '/../spec_helper'

# Language-level method behaviour
describe "Defining methods" do
  it "one more time redefines a method" do
    def barfoo; 100; end

    barfoo.should == 100

    def barfoo; 200; end

    barfoo.should == 200
  end
end

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
  
  it "with lambda as block argument is ok" do
    def foo(a,&b); [a,yield(b)] end

    l = lambda { 300 }
    foo(10, &l).should == [10,300]
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
      foo (0..n).map { }
    end

    mybar(10).should == nil
  end
end

describe "Defining methods with *" do
  it "If * by itself is the only param, method takes any number of args that are ignored" do
    def foo(*); end;

    foo.should == nil
    foo(1, 2).should == nil
    foo(1, 2, 3, 4, :a, :b, 'c', 'd').should == nil
  end

  it "With a parameter name, * stores all extra arguments as an Array in it" do
    def foo(*a); a; end;
    foo.should == []
    foo(1, 2).should == [1, 2]
    foo([:a]).should == [[:a]]
  end

  it "A * param may be preceded by any number of other parameter names" do
    def foo(a, b, c, d, e, *f); [a, b, c, d, e, f]; end
    foo(1, 2, 3, 4, 5, 6, 7, 8).should == [1, 2, 3, 4, 5, [6, 7, 8]]
  end
  
  it "allows to pass hashes without curly braces and still use *param" do
    def foo(a,b,*c); [a,b,c] end

    foo('abc', 'rbx' => 'cool', 'specs' => 'fail sometimes', 'oh' => 'shit', *[789, 'yeah']).
      should ==
      ['abc', { 'rbx' => 'cool', 'specs' => 'fail sometimes', 'oh' => 'shit'}, [789, 'yeah']]
  end
  
  it "allows to pass hashes without curly braces, a block argument and still use *param" do
    def foo(a,b,*c,&d); [a,b,c,yield(d)] end

    foo('abc', 'rbx' => 'cool', 'specs' => 'fail sometimes', 'oh' => 'shit', *[789, 'yeah']) { 3 }.
      should ==
      ['abc', { 'rbx' => 'cool', 'specs' => 'fail sometimes', 'oh' => 'shit'}, [789, 'yeah'], 3]
    
    foo('abc', 'rbx' => 'cool', 'specs' => 'fail sometimes', *[789, 'yeah']) do 3 end.should ==
      ['abc', { 'rbx' => 'cool', 'specs' => 'fail sometimes' }, [789, 'yeah'], 3]

    l = lambda { 3 }
    
    foo('abc', 'rbx' => 'cool', 'specs' => 'fail sometimes', *[789, 'yeah'], &l).should ==
      ['abc', { 'rbx' => 'cool', 'specs' => 'fail sometimes' }, [789, 'yeah'], 3]
  end

  it "Only one *param may appear in a parameter list" do
    lambda { eval 'def foo(a, *b, *c); end' }.should raise_error(SyntaxError)
  end

  it "The required arguments must be supplied even with a * param" do
    def foo(a, b, *c); end
    lambda { foo 1 }.should raise_error(ArgumentError)
  end
end

describe "Defining a method with a default arg" do
  it "assigns single default arg when nothing is passed" do
    def foo(a = 1)
      a
    end
    foo.should == 1
    foo(2).should == 2
  end

  it "assigns [] to unpassed rest args" do
    def foo(a = 1, *b)
      [a,b]
    end
    foo.should == [1, []]
    foo(2).should == [2, []]
  end

  it "assigns when only required args are passed" do
    def foo(a, b = 2)
      [a,b]
    end
    lambda { foo }.should raise_error(ArgumentError)
    foo(1).should == [1, 2]
  end

  it "assigns default and assign [] to rest args when only required args are present" do
    def foo(a, b = 2, *c)
      [a,b,c]
    end
    lambda { foo }.should raise_error(ArgumentError)
    foo(1).should == [1,2,[]]
  end

  it "does not assign when restargs get assigned" do
    def foo(a = 1, *args)
      [a,args]
    end
    foo(2,2).should == [2,[2]]
  end

  it "does not assign when required and restargs get assigned" do
    def foo(a, b = 2, *args)
      [a,b,args]
    end
    foo(2,3,3).should == [2,3,[3]]
  end
end

describe "Defining a singleton method" do
  it "works on an lvar" do
    a = "hi"
    def a.foo
      5
    end
    a.foo.should == 5
  end

  it "works on an ivar" do
    @a = "hi"
    def @a.foo
      6
    end
    @a.foo.should == 6
  end

  it "works on a gvar" do
    $__a__ = "hi"
    def $__a__.foo
      7
    end
    $__a__.foo.should == 7
  end

  it "works on a cvar" do
    @@a = "hi"
    def @@a.foo
      8
    end
    @@a.foo.should == 8
  end

  it "works without a body" do
    class DefSpec
      def self.foo;end
    end
    DefSpec.foo.should == nil
  end
end

describe "Defining a method with complex default args" do
  it "lets you define a method inside a default argument" do
    class DefSpecs
      def foo(x = (def foo; "hello"; end;1));x;end
    end
    d = DefSpecs.new
    d.foo(42).should == 42
    d.foo.should == 1
    d.foo.should == 'hello'
  end

  it "lets you use an fcall as a default argument" do
    def foo(x = caller())
      x
    end
    foo.shift.class.should == String
  end

  it "evaluates default arguments in the proper scope" do
    def foo(x = ($foo_self = self; nil)); end
    foo
    $foo_self.should == self
  end

  it "supports method calls on other arguments as defaults" do
    def foo(obj, width=obj.length)
      width
    end
    foo('abcde').should == 5
  end

  it "supports procs as defaults" do
    def foo(output = 'a', prc = lambda {|n| output * n})
      prc.call(5)
    end
    foo.should == 'aaaaa' 
  end
end

describe "Defining a singleton method with complex default args" do
  it "lets you define a method inside a default argument" do
    $__a = "hi"
    def $__a.foo(x = (def $__a.foo; "hello"; end;1));x;end
    $__a.foo(42).should == 42
    $__a.foo.should == 1
    $__a.foo.should == 'hello'
  end

  it "lets you use an fcall as a default argument" do
    a = "hi"
    def a.foo(x = caller())
      x
    end
    a.foo.shift.class.should == String
  end

  it "evaluates default arguments in the proper scope" do
    a = "hi"
    def a.foo(x = ($foo_self = self; nil)); 5 ;end
    a.foo
    $foo_self.should == a
  end

  it "supports method calls on other arguments as defaults" do
    a = 'hi'
    def a.foo(obj, width=obj.length)
      width
    end
    a.foo('abcde').should == 5
  end
  
  it "supports procs as defaults" do
    a = 'hi'
    def a.foo(output = 'a', prc = lambda {|n| output * n})
      prc.call(5)
    end
    a.foo.should == 'aaaaa' 
  end
end

