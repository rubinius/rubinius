require File.dirname(__FILE__) + '/../spec_helper'
require File.dirname(__FILE__) + '/../mini_mock'

# DO NOT PUT ANYTHING ABOVE THIS
context 'Examining the call stack' do
  it "may be viewed through .caller" do
    def a(skip)
      caller(skip)
    end
    def b(skip)
      a(skip)
    end
    def c(skip)
      b(skip)
    end

    c(0)[0][-23..-1].should == "kernel_spec.rb:8:in `a'"
    c(0)[1][-24..-1].should == "kernel_spec.rb:11:in `b'"
    c(0)[2][-24..-1].should == "kernel_spec.rb:14:in `c'"
  end
end

context "Kernel.Float()" do
  specify "should call to_f to convert any arbitrary argument to a Float" do
    class KernelSpecFloat
      def to_f; 1.1; end
    end

    Float(KernelSpecFloat.new).should == 1.1
  end

 specify "should call to_i to convert any arbitrary argument to a Float" do
   class KernelSpecFloat2
     def to_i; 7; end
   end

   Float(KernelSpecFloat2.new).should == 7.0
 end

  specify "should give to_f precedence over to_i" do
    class KernelSpecFloat3
      def to_i; 7; end
      def to_f; 69.9; end
    end

    Float(KernelSpecFloat3.new).should == 69.9
  end

  specify "should raise a TypeError if there is no to_f or to_i method on an object" do
    class KernelSpecFloat4; end

    should_raise(TypeError) { Float(KernelSpecFloat4.new) }
  end

  specify "should raise a TypeError if to_f doesn't return a Float" do
    class KernelSpecFloat5
      def to_f; 'har'; end
    end

    should_raise(TypeError) { Float(KernelSpecFloat5.new) }  
  end
end


context "Kernel.Integer()" do
  specify "should call to_i to convert any arbitrary argument to an Integer" do
    class KernelSpecInt
      def to_i; 7; end
    end

    Integer(KernelSpecInt.new).should == 7
  end

  specify "should raise a TypeError if there is no to_i method on an object" do
    class KernelSpecInt2; end

    should_raise(TypeError) { Integer(KernelSpecInt2.new) }
  end

  specify "should raise a TypeError if to_i doesn't return an Integer" do
    class KernelSpecInt3
      def to_i; 'har'; end
    end

    should_raise(TypeError) { Integer(KernelSpecInt3.new) }
  end
end


context "Kernel.Array()" do
  specify "should call to_a to convert any arbitrary argument to an Array" do
    class KernelSpecArray
      def to_a; [:a, :b]; end
    end

    Array(KernelSpecArray.new).should == [:a, :b]
  end

  specify "should prefer to_ary over to_a" do
    class KernelSpecArray2
      def to_ary; [:a, :r, :y]; end
      def to_a; [:a, :b]; end
    end
    Array(KernelSpecArray2.new).should == [:a, :r, :y]
  end

  specify "should raise a TypeError if to_a doesn't return an Array" do
    class KernelSpecArray3
      def to_a; 'har'; end
    end

    should_raise(TypeError) { Array(KernelSpecArray3.new) }
  end

  specify "called with nil as argument should return an empty Array" do
    Array(nil).should == []
  end
end


context "Kernel.String()" do
  specify "should call to_s to convert any arbitrary object to an String" do
    class KernelSpecString
      def to_s; "bar"; end
    end

    String(3).should == '3'
    String("foo").should == 'foo'
    String(nil).should == ''
    String(KernelSpecString.new).should == 'bar'
  end
end

context "Kernel.at_exit()" do
  specify "should fire after all other code" do
    result = `#{RUBY_NAME} -e "at_exit {print 5}; print 6"`
    result.should == "65"
  end

  specify "should fire in reverse order of registration" do
    result = `#{RUBY_NAME} -e "at_exit {print 4};at_exit {print 5}; print 6; at_exit {print 7}"`
    result.should == '6754'
  end
end

# would be better to mock out raise, but that'd have to be done at rubinius compile time
context "Kernel.fail()" do
  specify "should raise an exception" do
    should_raise(RuntimeError) { fail }
  end

  specify "should accept an Object with an exception method returning an Exception" do
    class Boring
      def self.exception(msg)
        StandardError.new msg
      end
    end
    should_raise(StandardError) { fail Boring, "..." }
  end

  specify "should instantiate specified exception class" do
    class LittleBunnyFooFoo < RuntimeError; end
    should_raise(LittleBunnyFooFoo) { fail LittleBunnyFooFoo }
  end

  specify "should use specified message" do
    should_raise(RuntimeError) { 
      begin
        fail "the duck is not irish."
      rescue => e
        e.message.should == "the duck is not irish."
        raise
      else
        raise Exception
      end
    }
  end
end

context "Kernel.warn()" do
  class FakeErr
    def initialize; @written = ''; end
    def written; @written; end
    def write(warning); @written << warning; end;
   end

  specify "should call #write on $stderr" do
    s = $stderr
    $stderr = FakeErr.new
    warn("Ph'nglui mglw'nafh Cthulhu R'lyeh wgah'nagl fhtagn")
    $stderr.written.should == "Ph'nglui mglw'nafh Cthulhu R'lyeh wgah'nagl fhtagn\n"
    $stderr = s
  end

  specify "should write the default record seperator (\\n) and NOT $/ to $stderr after the warning message" do
    s = $stderr
    rs = $/
    $/ = 'rs'
    $stderr = FakeErr.new
    warn("")
    $stderr.written.should == "\n"
    $stderr = s
    $/ = rs
  end

  specify "should not call #write on $stderr if $VERBOSE is nil" do
    v = $VERBOSE
    $VERBOSE = nil
    s = $stderr
    $stderr = FakeErr.new
    warn("Ph'nglui mglw'nafh Cthulhu R'lyeh wgah'nagl fhtagn")
    $stderr.written.should == ""
    $stderr = s
    $VERBOSE = v
  end
end

context "Kernel.loop()" do
  specify "loop calls block until it is terminated by a break" do
    i = 0
    loop do
      i += 1
      break if i == 10
    end

    i.should == 10
  end

  specify "loop returns value passed to break" do
    loop do
      break 123
    end.should == 123
  end

  specify "loop returns nil if no value passed to break" do
    loop do
      break
    end.should == nil
  end

  specify "loop raises LocalJumpError if no block given" do
    should_raise(LocalJumpError) { loop }
  end
end

context "Kernel.srand()" do
  it "srand should return the previous seed value" do
    srand(10)
    srand(20).should == 10
  end

  it "srand should seed the RNG correctly and repeatably" do
    srand(10)
    x = rand
    srand(10)
    rand.should == x
  end
end

context "Kernel.rand()" do
  it "rand should have the random number generator seeded uniquely at startup" do
    `#{RUBY_NAME} -e "puts rand"`.should_not == `#{RUBY_NAME} -e "puts rand"`
  end

  it "rand should return a random float less than 1 if no max argument is passed" do
    rand.kind_of?(Float).should == true
  end

  it "rand should return a random int or bigint less than the argument for an integer argument" do
    rand(77).kind_of?(Integer).should == true
  end

  it "rand should return a random integer less than the argument casted to an int for a float argument greater than 1" do
    rand(1.3).kind_of?(Integer).should == true
  end

  it "rand should return a random float less than 1 for float arguments less than 1" do
    rand(0.01).kind_of?(Float).should == true
  end

  it "rand should never return a value greater or equal to 1.0 with no arguments" do
    1000.times do
      (rand < 1.0).should == true
    end
  end

  it "rand should never return a value greater or equal to any passed in max argument" do
    1000.times do
      (rand(100) < 100).should == true
    end
  end
end

describe "Kernel#instance_variable_get" do
  it "returns the value of the instance variable" do
    class Fred 
      def initialize(p1, p2) 
        @a, @b = p1, p2 
      end 
    end 
    fred = Fred.new('cat', 99) 
    fred.instance_variable_get(:@a).should == "cat"
    fred.instance_variable_get("@b").should == 99

    a = []
    a.instance_variable_set(:@c, 1)
    a.instance_variable_get(:@c).should == 1
  end

  it "returns nil if the instance variable does not exist" do
    [].instance_variable_get(:@c).should == nil
  end

  it "raises NameError exception if the argument is not of form '@x'" do
    class NoFred; end
    should_raise(NameError) { NoFred.new.instance_variable_get(:c) }
    should_raise(NameError) { [].instance_variable_get(:c) }
  end
end

describe "Kernel#instance_variable_set" do
  it "sets the value of the specified instance variable" do
    class Dog
      def initialize(p1, p2) 
        @a, @b = p1, p2 
      end 
    end 
    Dog.new('cat', 99).instance_variable_set(:@a, 'dog').should == "dog"
  end

  it "sets the value of the instance variable when no instance variables exist yet" do
    class NoVariables; end
    NoVariables.new.instance_variable_set(:@a, "new").should == "new"
  end

  it "raises NameError exception if the argument is not of form '@x'" do
    class NoDog; end
    should_raise(NameError) { NoDog.new.instance_variable_set(:c, "cat") }
  end
end

describe "Array inherited instance method" do
  it "instance_variable_get returns the value of the instance variable" do
    a = []
    a.instance_variable_set(:@c, 1)
    a.instance_variable_get(:@c).should == 1
  end

  it "instance_variable_get returns nil if the instance variable does not exist" do
    [].instance_variable_get(:@c).should == nil
  end

  it "instance_variable_get should raise NameError if the argument is not of form '@x'" do
    should_raise(NameError) { [].instance_variable_get(:c) }
  end
end


class KernelMethods
  def self.ichi; end
  def ni; end
  class << self
    def san; end
  end
  
  private
  
  def self.shi; end
  def juu_shi; end
  
  class << self
    def roku; end

    private
    
    def shichi; end
  end
  
  protected
  
  def self.hachi; end
  def ku; end
  
  class << self
    def juu; end
    
    protected
    
    def juu_ichi; end
  end
  
  public
  
  def self.juu_ni; end
  def juu_san; end
end

describe "Kernel#methods" do
  it "returns a list of the names of publicly accessible methods in the object" do
    KernelMethods.methods(false).sort.should ==
      ["hachi", "ichi", "juu", "juu_ichi", "juu_ni", "roku", "san", "shi"]
    KernelMethods.new.methods(false).should == []
  end
  
  it "returns a list of the names of publicly accessible methods in the object and its ancestors and mixed-in modules" do
    (KernelMethods.methods(false) & KernelMethods.methods).sort.should ==
      ["hachi", "ichi", "juu", "juu_ichi", "juu_ni", "roku", "san", "shi"]
    m = KernelMethods.new.methods
    m.should_include('ku')
    m.should_include('ni')
    m.should_include('juu_san')
  end
end

describe "Kernel#singleton_methods" do
  it "returns a list of the names of singleton methods in the object" do
    KernelMethods.singleton_methods(false).sort.should ==
      ["hachi", "ichi", "juu", "juu_ichi", "juu_ni", "roku", "san", "shi"]
    KernelMethods.new.singleton_methods(false).should == []
  end
  
  it "returns a list of the names of singleton methods in the object and its ancestors and mixed-in modules" do
    (KernelMethods.singleton_methods(false) & KernelMethods.singleton_methods).sort.should ==
      ["hachi", "ichi", "juu", "juu_ichi", "juu_ni", "roku", "san", "shi"]
    KernelMethods.new.singleton_methods.should == []
  end
end

describe "Kernel#public_methods" do
  it "returns a list of the names of publicly accessible methods in the object" do
    KernelMethods.public_methods(false).sort.should == 
      ["allocate", "hachi", "ichi", "juu", "juu_ni", "new", "roku", "san", "shi", "superclass"]
    KernelMethods.new.public_methods(false).sort.should == ["juu_san", "ni"]
  end
  
  it "returns a list of the names of publicly accessible methods in the object and its ancestors and mixed-in modules" do
    (KernelMethods.public_methods(false) & KernelMethods.public_methods).sort.should == 
      ["allocate", "hachi", "ichi", "juu", "juu_ni", "new", "roku", "san", "shi", "superclass"]
    m = KernelMethods.new.public_methods
    m.should_include('ni')
    m.should_include('juu_san')
  end
end

describe "Kernel#private_methods" do
  it "returns a list of the names of privately accessible methods in the object" do
    KernelMethods.private_methods(false).sort.should == 
      ["inherited", "initialize", "initialize_copy", "shichi"]
    KernelMethods.new.private_methods(false).sort.should == ["juu_shi"]
  end
  
  it "returns a list of the names of privately accessible methods in the object and its ancestors and mixed-in modules" do
    (KernelMethods.private_methods(false) & KernelMethods.private_methods).sort.should == 
      ["inherited", "initialize", "initialize_copy", "shichi"]
    m = KernelMethods.new.private_methods
    m.should_include('juu_shi')
  end
end

describe "Kernel#protected_methods" do
  it "returns a list of the names of protected methods accessible in the object" do
    KernelMethods.protected_methods(false).sort.should == ["juu_ichi"]
    KernelMethods.new.protected_methods(false).should == ["ku"]
  end
  
  it "returns a list of the names of protected methods accessible in the object and from its ancestors and mixed-in modules" do
    (KernelMethods.protected_methods(false) & KernelMethods.protected_methods).sort.should == ["juu_ichi"]
    KernelMethods.new.protected_methods.should_include('ku')
  end
end
