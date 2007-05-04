require File.dirname(__FILE__) + '/../spec_helper'

context "Module" do
  specify "instance_methods with false should exclude super class instance methods" do
    class Foo
      def foo
      end
    end
    Foo.instance_methods(false).should == ["foo"]
  end
  
  specify "const_defined? should return false if the name is not defined" do
    Object.const_defined?("Whee").should == false
  end
  
  specify "const_defined? should return true if the name is defined" do
    class Blah
      class Whee
      end
    end

    Object.const_defined?(:Object).should == true
    Blah.const_defined?("Whee").should == true
    Object.const_defined?("Blah").should == true
    Object.const_defined?("Zargle").should == false
  end
  
  specify "include should accept multiple arguments" do
    class Foo
      include Comparable, Enumerable
    end
    Foo.new.class.to_s.should == 'Foo'
  end
  
  specify "should provide append_features" do
    module Pod
      def self.append_features(mod)
        super(mod)
        mod.some_class_method
      end
    end

    class Foo
      def self.some_class_method
        @included = true
      end
      
      def self.included?
        @included
      end
      
      include Pod
    end
    
    Foo.included?.should == true
  end
        
  specify "append_features should include self in other module unless it is already included" do
    module Sod; end
    module Mod; end
    class Bar
      include Sod, Mod
    end
    Bar.ancestors.reject { |m| m.to_s.include?(':') }.inspect.should == '[Bar, Sod, Mod, Object, Kernel]'
  end
end

context "Module.new method" do

  specify "should return a new instance" do
    Module.new.class.should == Module
  end

  specify "may receive a block" do
    Module.new { self }.class.should == Module
  end

end

context "Module#module_eval given a block" do
  module Hello
    def en
      "hello"
    end
  end

  specify "should execute on the receiver context" do
    Hello.module_eval { name }.should == 'Hello'
  end

  specify "should bind self to the receiver module" do
    (Hello.object_id == Hello.module_eval { self.object_id }).should == true
  end

end

context "Module.define_method" do

  class Foo
    def foo
      "ok"
    end
  end
  @foo = Foo.new
  
  specify "should be private" do
    should_raise(NoMethodError) { Foo.define_method(:a) {  } }
  end

  specify "should receive an UnboundMethod" do
    Foo.module_eval do 
      define_method(:bar, instance_method(:foo))
    end
    @foo.bar.should == "ok"
  end

  specify "should receive a Method" do
    Foo.module_eval do
      define_method(:bar, Foo.new.method(:foo))
    end
    @foo.bar.should == "ok"
  end

  specify "should take a block with arguments" do
    Foo.module_eval do 
      define_method(:bar) { |what| "I love #{what}" }
    end
    @foo.bar("rubinius").should == "I love rubinius"
  end

  specify "should raise TypeError if not given a Proc/Method" do
    should_raise(TypeError) do
      Foo.module_eval do
        define_method(:bar, 1)
      end
    end
  end
  
end

context "Module" do
  module Moo
    def a; end
    def b; end
    def c; end
  end

  specify "should provide a method private that takes no arguments" do
    module Yauk
      private
      def a; end
    end
    Yauk.private_instance_methods.should == ["a"]
  end
  
  specify "should provide a method private that takes multiple arguments" do
    module Moo
      private :a, :b, :c
    end
    Moo.private_instance_methods.sort.should == ["a", "b", "c"]
  end
  
  specify "should provide a method protected that takes no arguments" do
    module Zar
      protected
      def a; end
    end
    Zar.protected_instance_methods.should == ["a"]
  end
  
  specify "should provide a method protected that takes multiple arguments" do
    module Moo
      protected :a, :b, :c
    end
    Moo.protected_instance_methods.sort.should == ["a", "b", "c"]
  end
  
  specify "should provide a method public that takes no arguments" do
    module Rilsk
      public
      def a; end
    end
    Rilsk.public_instance_methods.should == ["a"]
  end
  
  specify "should provide a method public that takes multiple arguments" do
    module Moo
      public :a, :b, :c
    end
    Moo.public_instance_methods.sort.should == ["a", "b", "c"]
  end
end
