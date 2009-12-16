require File.dirname(__FILE__) + '/../spec_helper'

# Why so many fixed arg tests?  JRuby and I assume other Ruby impls have
# separate call paths for simple fixed arity methods.  Testing up to five
# will verify special and generic arity code paths for all impls.
#
# Method naming conventions:
# P - Pre Required Args
# O - Optional Arg
# R - Rest Arg
# Q - Post Required Args (1.9)

describe "Calling a method" do
  it "with no arguments is ok" do
    def fooP0; 100 end

    fooP0.should == 100
  end

  it "with simple required arguments works" do
    def fooP1(a); [a]; end
    fooP1(1).should == [1]

    def fooP2(a,b); [a,b]; end
    fooP2(1,2).should == [1,2]

    def fooP3(a,b,c); [a,b,c]; end
    fooP3(1,2,3).should == [1,2,3]

    def fooP4(a,b,c,d); [a,b,c,d]; end
    fooP4(1,2,3,4).should == [1,2,3,4]

    def fooP5(a,b,c,d,e); [a,b,c,d,e]; end
    fooP5(1,2,3,4,5).should == [1,2,3,4,5]
  end

  it "works with optional arguments" do
    def fooP0O1(a=1); [a]; end
    fooP0O1().should == [1]

    def fooP1O1(a,b=1); [a,b]; end
    fooP1O1(1).should == [1,1]

    def fooP2O1(a,b,c=1); [a,b,c]; end
    fooP2O1(1,2).should == [1,2,1]

    def fooP3O1(a,b,c,d=1); [a,b,c,d]; end
    fooP3O1(1,2,3).should == [1,2,3,1]

    def fooP4O1(a,b,c,d,e=1); [a,b,c,d,e]; end
    fooP4O1(1,2,3,4).should == [1,2,3,4,1]

    def fooP0O2(a=1,b=2); [a,b]; end
    fooP0O2().should == [1,2]
  end

  it "works with rest arguments" do
    def fooP0R(*r); r; end
    fooP0R().should == []
    fooP0R(1).should == [1]
    fooP0R(1,2).should == [1, 2]

    def fooP1R(a, *r); [a, r]; end
    fooP1R(1).should == [1, []]
    fooP1R(1,2).should == [1, [2]]

    def fooP0O1R(a=1, *r); [a, r]; end
    fooP0O1R().should == [1, []]
    fooP0O1R(2).should == [2, []]
    fooP0O1R(2,3).should == [2, [3]]

    def fooP1O1R(a, b=1, *r); [a, b, r]; end
    fooP1O1R(1).should == [1, 1, []]
    fooP1O1R(1,2).should == [1, 2, []]
    fooP1O1R(1,2,3).should == [1, 2, [3]]
  end

  it "with an empty expression is like calling with nil argument" do
    def foo(a); a end
    foo(()).should be_nil
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
    def fooP3(a,b,c); [a,b,c] end

    fooP3(*[1,2,3]).should == [1,2,3]
  end

  it "with splat operator * and referenced array unpacks params" do
    def fooP3(a,b,c); [a,b,c] end

    a = [1,2,3]
    fooP3(*a).should == [1,2,3]
  end

  it "without parentheses works" do
    def fooP3(a,b,c); [a,b,c] end

    (fooP3 1,2,3).should == [1,2,3]
  end

  it "with a space separating method name and parenthesis treats expression in parenthesis as first argument" do
    def myfoo(x); x * 2 end
    def mybar
      # means myfoo((5).to_s)
      # NOT   (myfoo(5)).to_s
      myfoo (5).to_s
    end

    mybar().should == "55"
  end

  it "with invalid argument count raises an ArgumentError" do
    def fooP3(a,b,c); end

    lambda { fooP3 }.should raise_error(ArgumentError)
    lambda { fooP3(1,2) }.should raise_error(ArgumentError)
    lambda { fooP3(1,2,3,4) }.should raise_error(ArgumentError)
    lambda { fooP3((), (), (), ()) }.should raise_error(ArgumentError)
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

end

describe "Calling a private setter method" do
  describe "permits self as a receiver" do
    module MethodSpecs
      class PrivateSetter
        attr_reader :foo
        attr_writer :foo
        private :foo=

        def call_self_foo_equals(value)
          self.foo = value
        end

        def call_self_foo_equals_masgn(value)
          a, self.foo = 1, value
        end
      end
    end

    it "for normal assignment" do
      receiver = MethodSpecs::PrivateSetter.new
      receiver.call_self_foo_equals(42)
      receiver.foo.should == 42
    end

    it "for multiple assignment" do
      receiver = MethodSpecs::PrivateSetter.new
      receiver.call_self_foo_equals_masgn(42)
      receiver.foo.should == 42
    end
  end
end

describe "Calling a private getter method" do
  it "does not permit self as a receiver" do
    module MethodSpecs
      class PrivateGetter
        attr_reader :foo
        private :foo

        def call_self_foo
          self.foo
        end

        def call_self_foo_or_equals(value)
          self.foo ||= 6
        end
      end
    end

    receiver = MethodSpecs::PrivateGetter.new
    lambda { receiver.call_self_foo }.should raise_error(NoMethodError)
    lambda { receiver.call_self_foo_or_equals(6) }.should raise_error(NoMethodError)
  end
end

language_version __FILE__, "method"
