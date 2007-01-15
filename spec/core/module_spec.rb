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
    end.should == :Hello
  end

  specify "should bind self to the receiver module" do
    example(@src) do
      Hello.object_id == Hello.module_eval { self.object_id }
    end.should == true
  end

end

