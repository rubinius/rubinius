require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

autoload_file = File.dirname(__FILE__) + "/fixtures/autoload.rb"

describe "Module#autoload" do
  it "registers the given filename to be loaded the first time that the Module with the given name is accessed" do
    begin
      protect_loaded_features do
        $m = Module.new { autoload(:AAA, autoload_file) }
        # $m::AAA is set in module_spec.autoload.rb
        $m.const_get(:AAA).should == "test"
      end
      
      protect_loaded_features do
        $m = Module.new { autoload(:AAA, autoload_file) }
        $m.const_get(:AAA).should == "test"
      end
      
      protect_loaded_features do
        $m = Module.new { autoload(:AAA, autoload_file) }
        $m.const_get(:AAA).should == "test"
      end
        
    ensure
      $m = nil
    end
  end

  it "does not autoload when the specified constant was already set" do
    begin
      protect_loaded_features do
        $m = Module.new { autoload(:AAA, autoload_file) }
        $m.const_set(:AAA, "testing!")
        $m.const_get(:AAA).should == "testing!"
      end
    ensure
      $m = nil
    end
  end

  it "raises a NameError when an invalid constant name is given" do
    lambda {
      Module.new { autoload("no_constant", autoload_file) }
    }.should raise_error(NameError)

    lambda {
      Module.new { autoload("123invalid", autoload_file) }
    }.should raise_error(NameError)

    lambda {
      Module.new { autoload("This One is invalid, too!", autoload_file) }
    }.should raise_error(NameError)
  end
  
  it "raises an ArgumentError when an empty filename is given" do
    lambda { Module.new { autoload("A", "") } }.should raise_error(ArgumentError)
  end
  
  it "loads constants that are registered at toplevel" do
    ModuleSpecAutoloadToplevel.message.should == "success"
  end

  it "triggers an autoload before using a toplevel constant" do
    protect_loaded_features do
      class ModuleSpecs::AutoLoadParent
        autoload(:AutoLoadSubject, File.dirname(__FILE__) + "/fixtures/autoload_nested.rb")
      end

      class ModuleSpecs::AutoLoadChild < ModuleSpecs::AutoLoadParent
        ModuleSpecs::AutoLoadParent::AutoLoadSubject.message.should == "success"
      end
    end  
  end

  it "should not fail when the load path is manually required" do
    protect_loaded_features do
      module ModuleSpecs::AutoloadRequire
        fixture = File.dirname(__FILE__) + "/fixtures/autoload_require.rb"

        autoload(:ModuleSpecAutoloadRequire, fixture)
        require fixture
      end

      ModuleSpecs::AutoloadRequire::ModuleSpecAutoloadRequire.hello.should == "Hello, World!"
    end  
  end

  it "should correctly match paths with and without ruby file extensions" do
    protect_loaded_features do
      module ModuleSpecs::AutoloadExtension
        with_rb = File.dirname(__FILE__) + "/fixtures/autoload_extension.rb"
        without_rb = with_rb[0..-4]

        autoload(:Constant, without_rb)
        require with_rb
      end
      ModuleSpecs::AutoloadExtension::Constant.message.should == "success"
    end  
  end

  it "should allow multiple autoload requests to reference one file" do
    protect_loaded_features do
      module ModuleSpecs::AutoloadMulti
        rb = File.dirname(__FILE__) + "/fixtures/autoload_multi.rb"
        autoload :FirstConst, rb
        autoload :SecondConst, rb
      end

      module ModuleSpecs::AutoloadMulti
        FirstConst.name.should == 'ModuleSpecs::AutoloadMulti::FirstConst'
      end
    end
  end
  
  it "doesn't call 'require' nor 'load' dynamically" do
    begin
      Kernel.module_eval do
        alias x_require require
        alias x_load load
  
        def require *a
          raise Exception.new("require called")
        end
  
        def load *a
          raise Exception.new("load called")
        end
      end
      
      Module.new do
        $m = self
        
        protect_loaded_features do
          autoload(:AAA, autoload_file)
          $m.const_get(:AAA).should == "test"
        end
      end
    ensure
      Kernel.module_eval do
        alias require x_require
        alias load x_load
      end
    end
  end
  
  it "with constant enumeration or removal doesn't trigger file load" do
    protect_loaded_features do
      module ModuleSpecs::AutoloadTestModule1
        autoload(:X, File.dirname(__FILE__) + "/fixtures/autoload_detect.rb")
        constants.should == ["X"]
        const_defined?(:X).should == true
        defined?(X).should == "constant"
        remove_const(:X).should == nil
      end
    end  
  end
  
  it "uses dup copies for autoloaded constants" do
    protect_loaded_features do
      $m = Module.new do
        autoload(:AAA, autoload_file)
      end

      $n = $m.dup

      $m.autoload?(:AAA).should == autoload_file
      $n.autoload?(:AAA).should == autoload_file

      # trigger load:
      $m.const_get(:AAA)
      ruby_bug("redmine 620", "1.8.7.72") do
        $n.autoload?(:AAA).should == nil
        lambda { $n.const_get(:AAA) }.should raise_error(NameError) 
      end
    end    
  end

  it "autoloaded constants are removed on the first access" do
    protect_loaded_features do
      $m = Module.new do
        autoload(:AAA, File.dirname(__FILE__) + "/fixtures/autoload_detect_removal.rb")
      end
      $m.constants.should == ["AAA"]
      
      lambda { $m.const_get(:AAA) }.should raise_error(NameError) 
      
      $m.const_get(:Consts).should == []

      $m.constants.should == ["Consts"]
    end
  end
  
  ruby_bug("see [ruby-core:19127]", "1.8.7.72") do
    it "with constant lookup in class definition behaves correctly on autoloaded constant" do
      protect_loaded_features do
        module ModuleSpecs::AutoloadTestModule2
          lambda {class A
            autoload(:C, File.dirname(__FILE__) + "/fixtures/autoload_classes.rb")
            
            # should create a new class:
            
            # TODO: doesn't work in MRI
            class C
            end
            
          end}.should_not raise_error(NameError)
        end  
      end
    end
  end
end

describe "Module#autoload?" do
  it "returns the name of the file that will be autoloaded" do
    m = Module.new { autoload :AAA, "module_a" }
    m.autoload?(:AAA).should == "module_a"
    m.autoload?(:B).should == nil
  end
end
