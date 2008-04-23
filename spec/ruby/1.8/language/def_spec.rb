require File.dirname(__FILE__) + '/../spec_helper'

# Language-level method behaviour
describe "Redefining a method" do
  it "replaces the original method" do
    def barfoo; 100; end
    barfoo.should == 100

    def barfoo; 200; end
    barfoo.should == 200
  end
end

describe "An instance method definition with a splat" do
  it "accepts an unnamed '*' argument" do
    def foo(*); end;

    foo.should == nil
    foo(1, 2).should == nil
    foo(1, 2, 3, 4, :a, :b, 'c', 'd').should == nil
  end

  it "accepts a named * argument" do
    def foo(*a); a; end;
    foo.should == []
    foo(1, 2).should == [1, 2]
    foo([:a]).should == [[:a]]
  end

  it "accepts non-* arguments before the * argument" do
    def foo(a, b, c, d, e, *f); [a, b, c, d, e, f]; end
    foo(1, 2, 3, 4, 5, 6, 7, 8).should == [1, 2, 3, 4, 5, [6, 7, 8]]
  end
  
  # TODO - Move this elsewhere
  it "creates a method that can be invoked with an inline hash argument" do
    def foo(a,b,*c); [a,b,c] end

    foo('abc', 'rbx' => 'cool', 'specs' => 'fail sometimes', 'oh' => 'shit', *[789, 'yeah']).
      should ==
      ['abc', { 'rbx' => 'cool', 'specs' => 'fail sometimes', 'oh' => 'shit'}, [789, 'yeah']]
  end
  
  it "creates a method that can be invoked with an inline hash and a block" do
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

  it "allows only a single * argument" do
    lambda { eval 'def foo(a, *b, *c); end' }.should raise_error(SyntaxError)
  end

  it "requires the presence of any arguments that precede the *" do
    def foo(a, b, *c); end
    lambda { foo 1 }.should raise_error(ArgumentError)
  end
end

describe "An instance method with a default argument" do
  it "evaluates the default when no arguments are passed" do
    def foo(a = 1)
      a
    end
    foo.should == 1
    foo(2).should == 2
  end

  it "assigns an empty Array to an unused splat argument" do
    def foo(a = 1, *b)
      [a,b]
    end
    foo.should == [1, []]
    foo(2).should == [2, []]
  end

  it "evaluates the default when required arguments precede it" do
    def foo(a, b = 2)
      [a,b]
    end
    lambda { foo }.should raise_error(ArgumentError)
    foo(1).should == [1, 2]
  end

  it "prefers to assign to a default argument before a splat argument" do
    def foo(a, b = 2, *c)
      [a,b,c]
    end
    lambda { foo }.should raise_error(ArgumentError)
    foo(1).should == [1,2,[]]
  end

  it "prefers to assign to a default argument when there are no required arguments" do
    def foo(a = 1, *args)
      [a,args]
    end
    foo(2,2).should == [2,[2]]
  end

  it "does not evaluate the default when passed a value and a * argument" do
    def foo(a, b = 2, *args)
      [a,b,args]
    end
    foo(2,3,3).should == [2,3,[3]]
  end
end

describe "A singleton method definition" do
  it "can be declared for a local variable" do
    a = "hi"
    def a.foo
      5
    end
    a.foo.should == 5
  end

  it "can be declared for an instance variable" do
    @a = "hi"
    def @a.foo
      6
    end
    @a.foo.should == 6
  end

  it "can be declared for a global variable" do
    $__a__ = "hi"
    def $__a__.foo
      7
    end
    $__a__.foo.should == 7
  end

  it "can be declared for a class variable" do
    @@a = "hi"
    def @@a.foo
      8
    end
    @@a.foo.should == 8
  end

  it "can be declared with an empty method body" do
    class DefSpec
      def self.foo;end
    end
    DefSpec.foo.should == nil
  end
end

describe "A method defined with extreme default arguments" do
  it "can redefine itself when the default is evaluated" do
    class DefSpecs
      def foo(x = (def foo; "hello"; end;1));x;end
    end
    d = DefSpecs.new
    d.foo(42).should == 42
    d.foo.should == 1
    d.foo.should == 'hello'
  end

  it "may use an fcall as a default" do
    def foo(x = caller())
      x
    end
    foo.shift.class.should == String
  end

  it "evaluates the defaults in the method's scope" do
    def foo(x = ($foo_self = self; nil)); end
    foo
    $foo_self.should == self
  end

  it "may use preceding arguments as defaults" do
    def foo(obj, width=obj.length)
      width
    end
    foo('abcde').should == 5
  end

  it "may use a lambda as a default" do
    def foo(output = 'a', prc = lambda {|n| output * n})
      prc.call(5)
    end
    foo.should == 'aaaaa' 
  end
end

describe "A singleton method defined with extreme default arguments" do
  it "may use a method definition as a default" do
    $__a = "hi"
    def $__a.foo(x = (def $__a.foo; "hello"; end;1));x;end
    $__a.foo(42).should == 42
    $__a.foo.should == 1
    $__a.foo.should == 'hello'
  end

  it "may use an fcall as a default" do
    a = "hi"
    def a.foo(x = caller())
      x
    end
    a.foo.shift.class.should == String
  end

  it "evaluates the defaults in the singleton scope" do
    a = "hi"
    def a.foo(x = ($foo_self = self; nil)); 5 ;end
    a.foo
    $foo_self.should == a
  end

  it "may use preceding arguments as defaults" do
    a = 'hi'
    def a.foo(obj, width=obj.length)
      width
    end
    a.foo('abcde').should == 5
  end
  
  it "may use a lambda as a default" do
    a = 'hi'
    def a.foo(output = 'a', prc = lambda {|n| output * n})
      prc.call(5)
    end
    a.foo.should == 'aaaaa' 
  end
end


