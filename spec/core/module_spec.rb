require File.dirname(__FILE__) + '/../spec_helper'

context "Module" do
  specify "instance_methods with false should exclude super class instance methods" do
    example do
      class Foo
        def foo
        end
      end

      Foo.instance_methods(false)
    end.should == ["foo"]
  end
  
  specify "const_defined? should return false if the name is not defined" do
    example do
      Object.const_defined?("Whee")
    end.should == false
  end
  
  specify "const_defined? should return true if the name is defined" do
    example do
      class Blah
        class Whee
        end
      end

      [ Object.const_defined?(:Object),
        Blah.const_defined?("Whee"), 
        Object.const_defined?("Blah::Whee"),
        Object.const_defined?("Blah::Zargle") ]
    end.should == [true, true, true, false]
  end
  
  specify "include should accept multiple arguments" do
    example do
      class Foo
        include Comparable, Enumerable
      end
      Foo.new.class.to_s
    end.should == 'Foo'
  end
  
  specify "should provide append_features" do
    example(<<-CODE
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
    CODE
    ) do
      Foo.included?
    end.should == true
  end
        
  specify "append_features should include self in other module unless it is already included" do
    example do
      module Sod; end
      module Mod; end
      class Bar
        include Sod, Mod
      end
      Bar.ancestors.reject { |m| m.to_s.include?(':') }.inspect
    end.should == '[Bar, Sod, Mod, Object, Kernel]'
  end
end

context "Module.new method" do

  specify "should return a new instance" do
    example do
      Module.new.class
    end.should == Module
  end

  specify "may receive a block" do
    example do
      Module.new { self }.class
    end.should == Module
  end

end

context "Module#module_eval given a block" do
  setup do
    @src = code do
      module Hello
        def en
          "hello"
        end
      end
    end
  end

  specify "should execute on the receiver context" do
    example(@src) do
      Hello.module_eval { name }
    end.should == 'Hello'
  end

  specify "should bind self to the receiver module" do
    example(@src) do
      Hello.object_id == Hello.module_eval { self.object_id }
    end.should == true
  end

end

context "Module.define_method" do

  setup do 
    @src = code do
      class Foo
        def foo
          "ok"
        end
      end
      
      @foo = Foo.new
    end
  end
  
  specify "should be private" do
    example(@src) do
      begin
        Foo.define_method(:a) {  }
      rescue NoMethodError
        "ok"
      end
    end.should == "ok"
  end

  specify "should receive an UnboundMethod" do
    example(@src) do
      Foo.module_eval do 
        define_method(:bar, instance_method(:foo))
      end

      @foo.bar
    end.should == "ok"
  end

  specify "should receive a Method" do
    example(@src)  do
      Foo.module_eval do
        define_method(:bar, Foo.new.method(:foo))
      end
      
      @foo.bar
    end.should == "ok"
  end

  specify "should take a block with arguments" do
    example(@src) do
      Foo.module_eval do 
        define_method(:bar) { |what| "I love #{what}" }
      end
      @foo.bar("rubinius")
    end.should == "I love rubinius"
  end

  specify "should raise TypeError if not given a Proc/Method" do
    example(@src) do
      begin
        Foo.module_eval do
          define_method(:bar, 1)
        end
      rescue TypeError
        "ok"
      end
    end.should == "ok"
  end
  
end

context "Module" do
  setup do
    @src = code do
      module Moo
        def a; end
        def b; end
        def c; end
      end
    end
  end
  
  specify "should provide a method private that takes no arguments" do
    example do
      module Yauk
        private
        def a; end
      end
      Yauk.private_instance_methods
    end.should == ["a"]
  end
  
  specify "should provide a method private that takes multiple arguments" do
    example(@src) do
      module Moo
        private :a, :b, :c
      end
      Moo.private_instance_methods
    end.should == ["a", "b", "c"]
  end
  
  specify "should provide a method protected that takes no arguments" do
    example do
      module Zar
        protected
        def a; end
      end
      Zar.protected_instance_methods
    end.should == ["a"]
  end
  
  specify "should provide a method protected that takes multiple arguments" do
    example(@src) do
      module Moo
        protected :a, :b, :c
      end
      Moo.protected_instance_methods
    end.should == ["a", "b", "c"]
  end
  
  specify "should provide a method public that takes no arguments" do
    example do
      module Rilsk
        public
        def a; end
      end
      Rilsk.public_instance_methods
    end.should == ["a"]
  end
  
  specify "should provide a method public that takes multiple arguments" do
    example(@src) do
      module Moo
        public :a, :b, :c
      end
      Moo.public_instance_methods
    end.should == ["a", "b", "c"]
  end
end
