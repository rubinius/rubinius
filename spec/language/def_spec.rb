require File.dirname(__FILE__) + '/../spec_helper'

# Language-level method behaviour


describe 'Defining methods with *' do
  it 'If * by itself is the only param, method takes any number of args that are ignored' do
    def foo(*); end;

    foo.should == nil
    foo(1, 2).should == nil
    foo(1, 2, 3, 4, :a, :b, 'c', 'd').should == nil
  end

  it 'With a parameter name, * stores all extra arguments as an Array in it' do
    def foo(*a); a; end;
    foo.should == []
    foo(1, 2).should == [1, 2]
    foo([:a]).should == [[:a]]
  end

  it 'A * param may be preceded by any number of other parameter names' do
    def foo(a, b, c, d, e, *f); [a, b, c, d, e, f]; end
    foo(1, 2, 3, 4, 5, 6, 7, 8).should == [1, 2, 3, 4, 5, [6, 7, 8]]
  end

  it 'Only one *param may appear in a parameter list' do
    lambda { eval 'def foo(a, *b, *c); end' }.should raise_error(SyntaxError)
  end

  it 'The required arguments must be supplied even with a * param' do
    def foo(a, b, *c); end
    lambda { foo 1 }.should raise_error(ArgumentError)
  end
end

describe "Defining a method with a default arg" do
  it "should assign single default arg when nothing is passed" do
    def foo(a = 1)
      a
    end
    foo.should == 1
    foo(2).should == 2
  end

  it "should assign [] to unpassed rest args" do
    def foo(a = 1, *b)
      [a,b]
    end
    foo.should == [1, []]
    foo(2).should == [2, []]
  end

  it "should assign when only required args are passed" do
    def foo(a, b = 2)
      [a,b]
    end
    lambda { foo }.should raise_error(ArgumentError)
    foo(1).should == [1, 2]
  end

  it "should assign default and assign [] to rest args when only required args are present" do
    def foo(a, b = 2, *c)
      [a,b,c]
    end
    lambda { foo }.should raise_error(ArgumentError)
    foo(1).should == [1,2,[]]
  end

  it "should not assign when restargs get assigned" do
    def foo(a = 1, *args)
      [a,args]
    end
    foo(2,2).should == [2,[2]]
  end

  it "should not assign when required and restargs get assigned" do
    def foo(a, b = 2, *args)
      [a,b,args]
    end
    foo(2,3,3).should == [2,3,[3]]
  end
end

describe "Defining a singleton method" do
  it "should work on an lvar" do
    a = "hi"
    def a.foo
      5
    end
    a.foo.should == 5
  end

  it "should work on an ivar" do
    @a = "hi"
    def @a.foo
      6
    end
    @a.foo.should == 6
  end

  it "should work on a gvar" do
    $__a__ = "hi"
    def $__a__.foo
      7
    end
    $__a__.foo.should == 7
  end

  it "should work on a cvar" do
    @@a = "hi"
    def @@a.foo
      8
    end
    @@a.foo.should == 8
  end

  it "should work without a body" do
    class DefSpec
      def self.foo;end
    end
    DefSpec.foo.should == nil
  end
end

describe "Defining a method with complex default args" do
  it "should let you define a method inside a default argument" do
    class DefSpecs
      def foo(x = (def foo; "hello"; end;1));x;end
    end
    d = DefSpecs.new
    d.foo(42).should == 42
    d.foo.should == 1
    d.foo.should == 'hello'
  end

  it "should let you use an fcall as a default argument" do
    def foo(x = caller())
      x
    end
    foo.shift.class.should == String
  end

  it "should evaluate default arguments in the proper scope" do
    def foo(x = ($foo_self = self; nil)); end
    foo
    $foo_self.should == self
  end

  it "should support method calls on other arguments as defaults" do
    def foo(obj, width=obj.length)
      width
    end
    foo('abcde').should == 5
  end

  it "should support procs as defaults" do
    def foo(output = 'a', prc = lambda {|n| output * n})
      prc.call(5)
    end
    foo.should == 'aaaaa' 
  end
end

describe "Defining a singleton method with complex default args" do
  it "should let you define a method inside a default argument" do
    $__a = "hi"
    def $__a.foo(x = (def $__a.foo; "hello"; end;1));x;end
    $__a.foo(42).should == 42
    $__a.foo.should == 1
    $__a.foo.should == 'hello'
  end

  it "should let you use an fcall as a default argument" do
    a = "hi"
    def a.foo(x = caller())
      x
    end
    a.foo.shift.class.should == String
  end

  it "should evaluate default arguments in the proper scope" do
    a = "hi"
    def a.foo(x = ($foo_self = self; nil)); 5 ;end
    a.foo
    $foo_self.should == a
  end

  it "should support method calls on other arguments as defaults" do
    a = 'hi'
    def a.foo(obj, width=obj.length)
      width
    end
    a.foo('abcde').should == 5
  end
  
  it "should support procs as defaults" do
    a = 'hi'
    def a.foo(output = 'a', prc = lambda {|n| output * n})
      prc.call(5)
    end
    a.foo.should == 'aaaaa' 
  end
end

