require File.expand_path('../../spec_helper', __FILE__)
require File.expand_path('../fixtures/method', __FILE__)

# Why so many fixed arg tests?  JRuby and I assume other Ruby impls have
# separate call paths for simple fixed arity methods.  Testing up to five
# will verify special and generic arity code paths for all impls.
#
# Method naming conventions:
# P - Pre Required Args
# O - Optional Arg
# R - Rest Arg
# Q - Post Required Args (1.9)

specs = LangMethodSpecs

describe "Calling a method" do
  it "with no arguments is ok" do
    specs.fooP0.should == 100
  end

  it "with simple required arguments works" do
    specs.fooP1(1).should == [1]

    specs.fooP2(1,2).should == [1,2]

    specs.fooP3(1,2,3).should == [1,2,3]

    specs.fooP4(1,2,3,4).should == [1,2,3,4]

    specs.fooP5(1,2,3,4,5).should == [1,2,3,4,5]
  end

  it "works with optional arguments" do
    specs.fooP0O1().should == [1]

    specs.fooP1O1(1).should == [1,1]

    specs.fooP2O1(1,2).should == [1,2,1]

    specs.fooP3O1(1,2,3).should == [1,2,3,1]

    specs.fooP4O1(1,2,3,4).should == [1,2,3,4,1]

    specs.fooP0O2().should == [1,2]
  end

  it "works with rest arguments" do
    specs.fooP0R().should == []
    specs.fooP0R(1).should == [1]
    specs.fooP0R(1,2).should == [1, 2]

    specs.fooP1R(1).should == [1, []]
    specs.fooP1R(1,2).should == [1, [2]]

    specs.fooP0O1R().should == [1, []]
    specs.fooP0O1R(2).should == [2, []]
    specs.fooP0O1R(2,3).should == [2, [3]]

    specs.fooP1O1R(1).should == [1, 1, []]
    specs.fooP1O1R(1,2).should == [1, 2, []]
    specs.fooP1O1R(1,2,3).should == [1, 2, [3]]
  end

  it "with an empty expression is like calling with nil argument" do
    specs.one(()).should be_nil
  end

  it "with block as block argument is ok" do
    specs.oneb(10) do 200 end.should == [10,200]
    specs.oneb(10) { 200 }.should == [10,200]
  end

  it "with block argument converts the block to proc" do
    specs.makeproc { "hello" }.call.should == "hello"
    specs.makeproc { "hello" }.should be_kind_of(Proc)

    specs.enclosing_method.should == :break_return_value
  end

  it "with an object uses 'to_proc' for coercion" do
    x = "my proc"
    class << x
      def to_proc; Proc.new {|y| self + y}; end
    end

    x = specs.makeproc(&x)
    x.call(" called").should == "my proc called"
    specs.yield_now(" yielded", &x).should == "my proc yielded"
  end
  it "fails with both lambda and block argument" do
    l = lambda { 300 }
    lambda {
      eval "specs.oneb(10, &l){ 42}"
    }.should raise_error(SyntaxError)
  end

  it "with same names as existing variables is ok" do
    foobar = 100

    def foobar; 200; end

    foobar.should == 100
    foobar().should == 200
  end

  it "with splat operator * and literal array unpacks params" do
    specs.fooP3(*[1,2,3]).should == [1,2,3]
  end

  it "with splat operator * and referenced array unpacks params" do
    a = [1,2,3]
    specs.fooP3(*a).should == [1,2,3]
  end

  it "without parentheses works" do
    (specs.fooP3 1,2,3).should == [1,2,3]
  end

  it "with a space separating method name and parenthesis treats expression in parenthesis as first argument" do
    specs.weird_parens().should == "55"
  end

  it "with invalid argument count raises an ArgumentError" do
    lambda { specs.fooP3 }.should raise_error(ArgumentError)
    lambda { specs.fooP3(1,2) }.should raise_error(ArgumentError)
    lambda { specs.fooP3(1,2,3,4) }.should raise_error(ArgumentError)
    lambda { specs.fooP3((), (), (), ()) }.should raise_error(ArgumentError)
  end

  # "Allows infinite arguments" is kinda hard to spec
  it "allows any number of args beyond required to method with a splat" do
    a = Array.new(2500) { Object.new }
    obj = a[-1]

    lambda { specs.twos 1 }.should raise_error(ArgumentError)

    res = specs.twos 1, 2
    res.first.should == 0
    res.last.nil?.should == true

    res = specs.twos 1, 2, 3
    res.first.should == 1
    res.last.should == 3

    res = specs.twos 1, 2, 3, *a
    res.first.should == 2501
    res.last.should equal(obj)
  end

  ruby_version_is "" ... "1.9" do
    describe "allows []=" do
      before :each do
        @obj = LangMethodSpecs::AttrSet.new
      end

      it "with *args in the [] expanded to individual arguments" do
        ary = [2,3]
        (@obj[1, *ary] = 4).should == 4
        @obj.result.should == [1,2,3,4]
      end

      it "with multiple *args" do
        ary = [2,3]
        post = [4,5]
        (@obj[1, *ary] = *post).should == [4,5]
        @obj.result.should == [1,2,3,[4,5]]
      end

      it "with multiple *args and unwraps the last splat" do
        ary = [2,3]
        post = [4]
        (@obj[1, *ary] = *post).should == 4
        @obj.result.should == [1,2,3,4]
      end

      it "with a *args and multiple rhs args" do
        ary = [2,3]
        (@obj[1, *ary] = 4, 5).should == [4,5]
        @obj.result.should == [1,2,3,[4,5]]
      end
    end
  end

  it "allows to pass literal hashes without curly braces as the last parameter" do
    specs.fooP3('abc', 456, 'rbx' => 'cool',
          'specs' => 'fail sometimes', 'oh' => 'weh').should == \
      ['abc', 456, {'rbx' => 'cool', 'specs' => 'fail sometimes', 'oh' => 'weh'}]

    (specs.fooP3 'abc', 456, 'rbx' => 'cool',
         'specs' => 'fail sometimes', 'oh' => 'weh').should == \
      ['abc', 456, { 'rbx' => 'cool', 'specs' => 'fail sometimes', 'oh' => 'weh'}]
  end

  it "allows to literal hashes without curly braces as the only parameter" do
    specs.fooP1(:rbx => :cool, :specs => :fail_sometimes).should ==
      [{ :rbx => :cool, :specs => :fail_sometimes }]

    (specs.fooP1 :rbx => :cool, :specs => :fail_sometimes).should ==
      [{ :rbx => :cool, :specs => :fail_sometimes }]
  end

end

describe "Calling a private setter method" do
  describe "permits self as a receiver" do
    it "for normal assignment" do
      receiver = LangMethodSpecs::PrivateSetter.new
      receiver.call_self_foo_equals(42)
      receiver.foo.should == 42
    end

    it "for multiple assignment" do
      receiver = LangMethodSpecs::PrivateSetter.new
      receiver.call_self_foo_equals_masgn(42)
      receiver.foo.should == 42
    end
  end
end

describe "Calling a private getter method" do
  it "does not permit self as a receiver" do
    receiver = LangMethodSpecs::PrivateGetter.new
    lambda { receiver.call_self_foo }.should raise_error(NoMethodError)
    lambda { receiver.call_self_foo_or_equals(6) }.should raise_error(NoMethodError)
  end
end

language_version __FILE__, "method"
