require File.dirname(__FILE__) + '/../spec_helper'

context "Module" do
  specify "const_defined? should return false if the name is not defined" do
    Object.const_defined?("Whee").should == false
  end
  
  specify "const_defined? should return true if the name is defined" do
    class C
      class D
      end
    end

    Object.const_defined?(:Object).should == true
    C.const_defined?("D").should == true
    Object.const_defined?("C").should == true
    Object.const_defined?("Zargle").should == false
  end

  specify "const_set should create a constant" do
    Object.const_defined?("Cozzy").should == false
    Object.const_set("Cozzy", 'a constant!')
    Object.const_defined?("Cozzy").should == true
    Cozzy.should == 'a constant!'

    class Tenacious; end

    Tenacious.const_defined?(:D).should == false
    Tenacious.const_set(:D, 'The D')
    Tenacious.const_defined?(:D).should == true
    Tenacious::D.should == 'The D'
  end

  specify "const_set should fail on a bad constant name" do
    should_raise(NameError) { Object.const_set("cozzy", true) }
    should_raise(ArgumentError) { Object.const_set(2, true) }
    should_raise(TypeError) { Object.const_set(Time.now, true) }
  end

  specify "const_get should return a constant's value" do
    Object.const_get(:Object).should == Object
    Object.const_set(:Cozzer, 'cozzer!')
    Object.const_get(:Cozzer).should == 'cozzer!'
  end

  specify "const_get should fail on a bad constant name" do
    should_raise(NameError) { Object.const_get("cozzy") }
    should_raise(ArgumentError) { Object.const_get(2) }
    should_raise(TypeError) { Object.const_get(Time.now) }
  end
  
  specify "include should accept multiple arguments" do
    class E
      include Comparable, Enumerable
    end
    E.new.class.to_s.should == 'E'
  end
  
  specify "should provide append_features" do
    module F
      def self.append_features(mod)
        super(mod)
        mod.some_class_method
      end
    end

    class G
      def self.some_class_method
        @included = true
      end
      
      def self.included?
        @included
      end
      
      include F
    end
    
    G.included?.should == true
  end
        
  specify "append_features should include self in other module unless it is already included" do
    module H; end
    module I; end
    class J
      include H, I
    end
    J.ancestors.reject { |m| m.to_s.include?(':') }.inspect.should == '[J, H, I, Object, Kernel]'
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
  module K
    def en
      "hello"
    end
  end

  specify "should execute on the receiver context" do
    K.module_eval { name }.should == 'K'
  end

  specify "should bind self to the receiver module" do
    (K.object_id == K.module_eval { self.object_id }).should == true
  end

end

context "Module.module_function with arguments" do
  module TestModule_ModuleFuncWithArgs
    def foo;  :foo  end
    def bar;  :bar  end
    def baz;  :baz  end
    def quux; :quux end
    def zool; :zool end
    module_function :foo, :bar, :zool
  end

  mod = TestModule_ModuleFuncWithArgs

  specify "should allow calling module functions as instance functions" do
    mod.should_receive :method_missing, {:count => 2}
    mod.foo.should == :foo
    mod.bar.should == :bar
    mod.zool.should == :zool
    mod.baz
    mod.quux
  end

  module TestModule_ModuleFuncWithArgs
    def foo;  :oof  end
    def zool; :looz end
  end

  specify "should create the module functions as clones" do
    mod.foo.should == :foo
    mod.zool.should == :zool
  end

  class TestClass_ModuleFuncWithArgs
    include TestModule_ModuleFuncWithArgs
  end
  obj = TestClass_ModuleFuncWithArgs.new

  specify "should make module function instance methods private" do
    should_raise(NoMethodError) { obj.bar }
  end

  specify "should leave other methods as module methods" do
    # Private
    obj.instance_eval do
      bar.should == :bar
    end

    obj.baz.should == :baz
    obj.quux.should == :quux
    
    obj.foo.should == :oof
    obj.zool.should == :looz
  end
end

context "Module.module_function without arguments" do
  module TestModule_ModuleFuncNoArgs
    def baz;  :baz  end
    def quux; :quux end

    module_function
    def foo;  :foo  end
    def bar;  :bar  end
    def zool; :zool end
  end
  
  mod = TestModule_ModuleFuncNoArgs

  specify "should allow calling module functions as instance functions" do
    mod.should_receive :method_missing, {:count => 2}
    mod.foo.should == :foo
    mod.bar.should == :bar
    mod.zool.should == :zool
    mod.baz
    mod.quux
  end

  module TestModule_ModuleFuncNoArgs
    def foo;  :oof  end
    def zool; :looz end
  end

  specify "should create the instance functions as clones" do
    mod.foo.should == :foo
    mod.zool.should == :zool
  end

  class TestClass_ModuleFuncNoArgs
    include TestModule_ModuleFuncNoArgs
  end
  obj = TestClass_ModuleFuncNoArgs.new

  specify "should make module function instance methods private" do
    should_raise(NoMethodError) { obj.bar }
  end

  specify "should leave other methods as module methods" do
    # Private
    obj.instance_eval do
      bar.should == :bar
    end

    obj.baz.should == :baz
    obj.quux.should == :quux    
    
    obj.foo.should == :oof
    obj.zool.should == :looz
  end

  module TestModule_ModuleFuncNoArgs
    def mri;  :mri end
    module_function :mri
    def odd;  :odd end
  end

  specify "should finish the default aliasing after passing an arg" do
    mod.should_receive :method_missing
    mod.mri.should == :mri
    mod.odd
  end
end

context "Module.define_method" do
  class L
    def foo
      "ok"
    end
  end
  
  specify "should be private" do
    should_raise(NoMethodError) { L.define_method(:a) {  } }
  end

  specify "should receive an UnboundMethod" do
    L.module_eval do 
      define_method(:bar, instance_method(:foo))
    end
    L.new.bar.should == 'ok'
  end

  specify "should receive a Method" do
    L.module_eval do
      define_method(:bar, L.new.method(:foo))
    end
    L.new.bar.should == 'ok'
  end

  specify "should take a block with an argument" do
    L.module_eval do 
      define_method(:bak) { |what| "I love #{what}" }
    end
    L.new.bak("rubinius").should == "I love rubinius"
  end

  specify "should take a block with multiple arguments" do
    L.module_eval do 
      define_method(:baz) { |what, how_much| "I love #{what} #{how_much}" }
    end
    L.new.baz("rubinius", 'a whole lot').should == "I love rubinius a whole lot"
  end

  specify "should take a variadic block" do
    L.module_eval do 
      define_method(:dots) { |*args| args.join('.') }
    end
    L.new.dots(1,2,3,4,5,6).should == "1.2.3.4.5.6"
  end

  specify "should raise TypeError if not given a Proc/Method" do
    should_raise(TypeError) do
      L.module_eval do
        define_method(:bar, 1)
      end
    end
  end
  
end

context "Module visibility methods (private, protected, public)" do
  specify "should work without args" do
    module TestModule_VisibilityNoArgs
      private
      def private1; end
      def private2; end
      
      protected
      def protected1; end
      def protected2; end
      
      public
      def public1; end
      def public2; end

      private_instance_methods.sort.should == ["private1", "private2"]
      protected_instance_methods.sort.should == ["protected1", "protected2"]
      public_instance_methods.sort.should == ["public1", "public2"]
    end
  end
  
  specify "should work with args" do
    module TestModule_VisibilityNoArgs
      def private1; end
      def private2; end
      private :private1, :private2
      
      def protected1; end
      def protected2; end
      protected :protected1, :protected2
      
      def public1; end
      def public2; end
      public :public1, :public2

      private_instance_methods.sort.should == ["private1", "private2"]
      protected_instance_methods.sort.should == ["protected1", "protected2"]
      public_instance_methods.sort.should == ["public1", "public2"]
    end
  end
  
  # TODO: What about mixing the two styles?
end
