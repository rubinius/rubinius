require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Module#autoload?" do
  it "returns the name of the file that will be autoloaded" do
    ModuleSpecs::Autoload.autoload :Autoload, "autoload.rb"
    ModuleSpecs::Autoload.autoload?(:Autoload).should == "autoload.rb"
  end

  it "returns nil if no file has been registered for a constant" do
    ModuleSpecs::Autoload.autoload?(:Manualload).should be_nil
  end
end

describe "Module#autoload" do
  before :all do
    @non_existent = fixture __FILE__, "no_autoload.rb"
  end

  before :each do
    @loaded_features = $".dup

    ScratchPad.clear
  end

  after :each do
    $".replace @loaded_features
  end

  it "registers a file to load the first time the named constant is accessed" do
    ModuleSpecs::Autoload.autoload :A, @non_existent
    ModuleSpecs::Autoload.autoload?(:A).should == @non_existent
  end

  it "sets the autoload constant in the constants table" do
    ModuleSpecs::Autoload.autoload :B, @non_existent
    ModuleSpecs::Autoload.should have_constant(:B)
  end

  it "loads the registered constant when it is accessed" do
    ModuleSpecs::Autoload.should_not have_constant(:X)
    ModuleSpecs::Autoload.autoload :X, fixture(__FILE__, "autoload_x.rb")
    ModuleSpecs::Autoload::X.should == :x
  end

  it "loads the registered constant into a dynamically created class" do
    cls = Class.new { autoload :C, fixture(__FILE__, "autoload_c.rb") }
    ModuleSpecs::Autoload::DynClass = cls

    ScratchPad.recorded.should be_nil
    ModuleSpecs::Autoload::DynClass::C.new.loaded.should == :dynclass_c
    ScratchPad.recorded.should == :loaded
  end

  it "loads the registered constant into a dynamically created module" do
    mod = Module.new { autoload :D, fixture(__FILE__, "autoload_d.rb") }
    ModuleSpecs::Autoload::DynModule = mod

    ScratchPad.recorded.should be_nil
    ModuleSpecs::Autoload::DynModule::D.new.loaded.should == :dynmodule_d
    ScratchPad.recorded.should == :loaded
  end

  it "loads the registered constant when it is opened as a class" do
    ModuleSpecs::Autoload.autoload :E, fixture(__FILE__, "autoload_e.rb")
    class ModuleSpecs::Autoload::E
    end
    ModuleSpecs::Autoload::E.new.loaded.should == :autoload_e
  end

  it "loads the registered constant when it is opened as a module" do
    ModuleSpecs::Autoload.autoload :F, fixture(__FILE__, "autoload_f.rb")
    module ModuleSpecs::Autoload::F
    end
    ModuleSpecs::Autoload::F.loaded.should == :autoload_f
  end

  it "loads the registered constant when it is inherited from" do
    ModuleSpecs::Autoload.autoload :G, fixture(__FILE__, "autoload_g.rb")
    class ModuleSpecs::Autoload::Gsub < ModuleSpecs::Autoload::G
    end
    ModuleSpecs::Autoload::Gsub.new.loaded.should == :autoload_g
  end

  it "loads the registered constant when it is included" do
    ModuleSpecs::Autoload.autoload :H, fixture(__FILE__, "autoload_h.rb")
    class ModuleSpecs::Autoload::HClass
      include ModuleSpecs::Autoload::H
    end
    ModuleSpecs::Autoload::HClass.new.loaded.should == :autoload_h
  end

  it "does not load the file when the constant is already set" do
    ModuleSpecs::Autoload.autoload :I, fixture(__FILE__, "autoload_i.rb")
    ModuleSpecs::Autoload.const_set :I, 3
    ModuleSpecs::Autoload::I.should == 3
    ScratchPad.recorded.should be_nil
  end

  it "loads a file with .rb extension when passed the name without the extension" do
    ModuleSpecs::Autoload.autoload :J, fixture(__FILE__, "autoload_j")
    ModuleSpecs::Autoload::J.should == :autoload_j
  end

  it "does not load the file if the file is manually required" do
    filename = fixture(__FILE__, "autoload_k.rb")
    ModuleSpecs::Autoload.autoload :K, filename

    require filename
    ScratchPad.recorded.should == :loaded
    ScratchPad.clear

    ModuleSpecs::Autoload::K.should == :autoload_k
    ScratchPad.recorded.should be_nil
  end

  it "allows multiple autoload constants for a single file" do
    filename = fixture(__FILE__, "autoload_lm.rb")
    ModuleSpecs::Autoload.autoload :L, filename
    ModuleSpecs::Autoload.autoload :M, filename
    ModuleSpecs::Autoload::L.should == :autoload_l
    ModuleSpecs::Autoload::M.should == :autoload_m
  end

  ruby_version_is "" ... "1.9" do
    it "removes the constant from the constant table if load fails" do
      ModuleSpecs::Autoload.autoload :Fail, @non_existent
      ModuleSpecs::Autoload.should have_constant(:Fail)

      lambda { ModuleSpecs::Autoload::Fail }.should raise_error(LoadError)
      ModuleSpecs::Autoload.should_not have_constant(:Fail)
    end

    it "removes the constant from the constant table if the loaded files does not define it" do
      ModuleSpecs::Autoload.autoload :O, fixture(__FILE__, "autoload_o.rb")
      ModuleSpecs::Autoload.should have_constant(:O)

      lambda { ModuleSpecs::Autoload::O }.should raise_error(NameError)
      ModuleSpecs::Autoload.should_not have_constant(:O)
    end

    it "does not load the file when refering to the constant in defined?" do
      module ModuleSpecs::Autoload::Q
        autoload :R, fixture(__FILE__, "autoload.rb")
        defined?(R).should == "constant"
      end
      ModuleSpecs::Autoload::Q.should have_constant(:R)
    end
  end

  ruby_version_is "1.9" do
    it "does not remove the constant from the constant table if load fails" do
      ModuleSpecs::Autoload.autoload :Fail, @non_existent
      ModuleSpecs::Autoload.should have_constant(:Fail)

      lambda { ModuleSpecs::Autoload::Fail }.should raise_error(LoadError)
      ModuleSpecs::Autoload.should have_constant(:Fail)
    end

    it "does not remove the constant from the constant table if the loaded files does not define it" do
      ModuleSpecs::Autoload.autoload :O, fixture(__FILE__, "autoload_o.rb")
      ModuleSpecs::Autoload.should have_constant(:O)

      lambda { ModuleSpecs::Autoload::O }.should raise_error(NameError)
      ModuleSpecs::Autoload.should have_constant(:O)
    end

    it "does not load the file when refering to the constant in defined?" do
      module ModuleSpecs::Autoload::Q
        autoload :R, fixture(__FILE__, "autoload.rb")
        defined?(R).should be_nil
      end
      ModuleSpecs::Autoload::Q.should have_constant(:R)
    end
  end

  it "does not load the file when removing an autoload constant" do
    module ModuleSpecs::Autoload::Q
      autoload :R, fixture(__FILE__, "autoload.rb")
      remove_const :R
    end
    ModuleSpecs::Autoload::Q.should_not have_constant(:R)
  end

  it "does not load the file when accessing the constants table of the module" do
    ModuleSpecs::Autoload.autoload :P, @non_existent
    ModuleSpecs::Autoload.const_defined?(:P).should be_true
  end

  it "loads the file when opening a module that is the autoloaded constant" do
    module ModuleSpecs::Autoload::U
      autoload :V, fixture(__FILE__, "autoload_v.rb")

      class V
        X = get_value
      end
    end

    ModuleSpecs::Autoload::U::V::X.should == :autoload_uvx
  end

  # [ruby-core:19127]
  it "raises a NameError when the autoload file did not define the constant and a module is opened with the same name" do
    lambda do
      module ModuleSpecs::Autoload
        class W
          autoload :Y, fixture(__FILE__, "autoload_w.rb")

          class Y
          end
        end
      end
    end.should raise_error(NameError)
    ScratchPad.recorded.should == :loaded
  end

  it "raises an ArgumentError when an empty filename is given" do
    lambda { ModuleSpecs.autoload :A, "" }.should raise_error(ArgumentError)
  end

  it "raises a NameError when the constant name starts with a lower case letter" do
    lambda { ModuleSpecs.autoload "a", @non_existent }.should raise_error(NameError)
  end

  it "raises a NameError when the constant name starts with a number" do
    lambda { ModuleSpecs.autoload "1two", @non_existent }.should raise_error(NameError)
  end

  it "raises a NameError when the constant name has a space in it" do
    lambda { ModuleSpecs.autoload "a name", @non_existent }.should raise_error(NameError)
  end

  ruby_bug "redmine #620", "1.8.6.322" do
    it "shares the autoload request across dup'ed copies of modules" do
      filename = fixture(__FILE__, "autoload_t.rb")
      mod1 = Module.new { autoload :T, filename }
      ModuleSpecs::Autoload::S = mod1
      mod2 = mod1.dup

      mod1.autoload?(:T).should == filename
      mod2.autoload?(:T).should == filename

      mod1::T.should == :autoload_t
      lambda { mod2::T }.should raise_error(NameError)
    end
  end

  it "raises a TypeError if opening a class with a different superclass than the class defined in the autoload file" do
    ModuleSpecs::Autoload.autoload :Z, fixture(__FILE__, "autoload_z.rb")
    class ModuleSpecs::Autoload::ZZ
    end

    lambda do
      class ModuleSpecs::Autoload::Z < ModuleSpecs::Autoload::ZZ
      end
    end.should raise_error(TypeError)
  end

  it "raises a TypeError if not passed a String for the filename" do
    name = mock("autoload_name.rb")
    name.stub!(:to_s).and_return("autoload_name.rb")
    name.stub!(:to_str).and_return("autoload_name.rb")

    lambda { ModuleSpecs::Autoload.autoload :Str, name }.should raise_error(TypeError)
  end
end

describe "Module#autoload" do
  # It would be nice to check this with a simple obj.should_not_receive,
  # but getting at that obj is implementation specific. This method is the
  # least implementation specific because it inserts the method that raises
  # an exception into the dynamic lookup chain.
  before :all do
    module Kernel
      alias_method :original_require, :require
      alias_method :original_load,    :load

      def require(name)
        raise Exception, "Kernel#require called"
      end

      def load(name)
        raise Exception, "Kernel#load called"
      end
    end
  end

  after :all do
    module Kernel
      alias_method :require, :original_require
      alias_method :load,    :original_load
    end
  end

  it "does not call Kernel#require or Kernel#load dynamically" do
    ModuleSpecs::Autoload.autoload :N, fixture(__FILE__, "autoload_n.rb")
    ModuleSpecs::Autoload::N.should == :autoload_n
  end
end
