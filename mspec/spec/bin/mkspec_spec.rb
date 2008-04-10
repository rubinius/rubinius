require File.dirname(__FILE__) + '/../../spec_helper'
load File.dirname(__FILE__) + '/../../bin/mkspec'

describe "The -c, --constant CONSTANT option" do
  before :each do
    @options = OptionParser.new
    OptionParser.stub!(:new).and_return(@options)
    @script = MkSpec.new
    @config = @script.config
  end

  it "is enabled by #options" do
    @options.stub!(:on)
    @options.should_receive(:on).with("-c", "--constant CONSTANT",
      String, an_instance_of(String))
    @script.options
  end

  it "adds CONSTANT to the list of constants" do
    ["-c", "--constant"].each do |opt|
      @config[:constants] = []
      @script.options [opt, "Object"]
      @config[:constants].should include("Object")
    end
  end
end

describe "The -b, --base DIR option" do
  before :each do
    @options = OptionParser.new
    OptionParser.stub!(:new).and_return(@options)
    @script = MkSpec.new
    @config = @script.config
  end

  it "is enabled by #options" do
    @options.stub!(:on)
    @options.should_receive(:on).with("-b", "--base DIR",
      String, an_instance_of(String))
    @script.options
  end

  it "sets the base directory relative to which the spec directories are created" do
    ["-b", "--base"].each do |opt|
      @config[:base] = nil
      @script.options [opt, "superspec"]
      @config[:base].should == File.expand_path("superspec")
    end
  end
end

describe "The -r, --require LIBRARY option" do
  before :each do
    @options = OptionParser.new
    OptionParser.stub!(:new).and_return(@options)
    @script = MkSpec.new
    @config = @script.config
  end

  it "is enabled by #options" do
    @options.stub!(:on)
    @options.should_receive(:on).with("-r", "--require LIBRARY",
      String, an_instance_of(String))
    @script.options
  end

  it "adds CONSTANT to the list of constants" do
    ["-r", "--require"].each do |opt|
      @config[:requires] = []
      @script.options [opt, "libspec"]
      @config[:requires].should include("libspec")
    end
  end
end

describe MkSpec, "#options" do
  before :each do
    @options = OptionParser.new
    OptionParser.stub!(:new).and_return(@options)
    @script = MkSpec.new
  end

  it "parses the command line options" do
    @options.should_receive(:parse).with(["--this", "and", "--that"])
    @script.options ["--this", "and", "--that"]
  end

  it "parses ARGV unless passed other options" do
    @options.should_receive(:parse).with(ARGV)
    @script.options
  end

  it "prints help and exits if passed an unrecognized option" do
    @script.should_receive(:puts).exactly(3).times
    @script.should_receive(:exit)
    @script.options "--iunknown"
  end
end

module MkSpecs
  class A; end
  class B; end
end

describe MkSpec, "#map_constants" do
  before :each do
    @script = MkSpec.new
  end

  it "returns a list of class/module constants from a list of names" do
    @script.map_constants(["Object", "Kernel", "Float"]).should == [
      Object, Kernel, Float
    ]
  end

  it "returns a list of class/module constants from a list of constants" do
    @script.map_constants([Object, Kernel, Float]).should == [
      Object, Kernel, Float
    ]
  end

  it "prepends the module to the name if the module is provided" do
    @script.map_constants(["A", "B"], "MkSpecs").should == [
      MkSpecs::A, MkSpecs::B
    ]
  end

  it "prints a warning if the name is not a class or module" do
    @script.should_receive(:puts).with("Float::MAX is not a class or module, ignoring")
    @script.map_constants(["MAX"], "Float")
  end
end

describe MkSpec, "#create_directory" do
  before :each do
    @script = MkSpec.new
  end

  it "prints a warning if a file with the directory name exists" do
    File.should_receive(:exist?).and_return(true)
    File.should_receive(:directory?).and_return(false)
    Dir.should_not_receive(:mkdir)
    @script.should_receive(:puts).with("spec/class already exists and is not a directory.")
    @script.create_directory(Class, "spec").should == "spec/class"
  end

  it "returns the name of the directory if it already exists" do
    File.should_receive(:exist?).and_return(true)
    File.should_receive(:directory?).and_return(true)
    Dir.should_not_receive(:mkdir)
    @script.create_directory(Class, "spec").should == "spec/class"
  end
end

describe MkSpec, "#write_requires" do
  before :each do
    @script = MkSpec.new
    @script.config[:base] = "spec"

    @file = mock("file")
    File.stub!(:open).and_yield(@file)
  end

  it "writes the spec_helper require line" do
    @file.should_receive(:puts).with("require File.dirname(__FILE__) + '/../../../spec_helper'")
    @script.write_requires("spec/core/tcejbo", "spec/core/tcejbo/inspect_spec.rb")
  end

  it "writes require lines for each library specified on the command line" do
    @file.stub!(:puts)
    @file.should_receive(:puts).with("require File.dirname(__FILE__) + '/../../../spec_helper'")
    @file.should_receive(:puts).with("require 'complex'")
    @script.config[:requires] << 'complex'
    @script.write_requires("spec/core/tcejbo", "spec/core/tcejbo/inspect_spec.rb")
  end
end

describe MkSpec, "#write_spec" do
  before :each do
    @file = mock("file")
    @file.stub!(:puts)
    File.stub!(:open).and_yield(@file)

    @script = MkSpec.new
    @script.stub!(:puts)

    @response = mock("system command response")
    @response.stub!(:=~).and_return(false)
    @script.stub!(:`).and_return(@response)
  end

  it "checks if specs exist for the method if the spec file exists" do
    @script.should_receive(:`).with(
        "mspec/bin/mspec-run --dry-run -fs -e 'Object#inspect' spec/core/tcejbo/inspect_spec.rb")
    @script.write_spec("spec/core/tcejbo/inspect_spec.rb", "Object#inspect", true)
  end

  it "escapes the Regexp when checking for method name in the spec file output" do
    Regexp.should_receive(:escape).with("Array#[]=")
    @script.write_spec("spec/core/yarra/element_set_spec.rb", "Array#[]=", true)
  end

  it "returns nil if the spec file exists and contains a spec for the method" do
    @response.stub!(:=~).and_return(true)
    @script.write_spec("spec/core/tcejbo/inspect_spec.rb", "Object#inspect", true).should == nil
  end

  it "does not print the spec file name if it exists and contains a spec for the method" do
    @response.stub!(:=~).and_return(true)
    @script.should_not_receive(:puts)
    @script.write_spec("spec/core/tcejbo/inspect_spec.rb", "Object#inspect", true)
  end

  it "prints the spec file name if a template spec is written" do
    @script.should_receive(:puts).with("spec/core/tcejbo/inspect_spec.rb")
    @script.write_spec("spec/core/tcejbo/inspect_spec.rb", "Object#inspect", true)
  end

  it "writes a template spec to the file if the spec file does not exist" do
    @file.should_receive(:puts)
    @script.should_receive(:puts).with("spec/core/tcejbo/inspect_spec.rb")
    @script.write_spec("spec/core/tcejbo/inspect_spec.rb", "Object#inspect", false)
  end

  it "writes a template spec to the file if it exists but contains no spec for the method" do
    @response.should_receive(:=~).and_return(false)
    @file.should_receive(:puts)
    @script.should_receive(:puts).with("spec/core/tcejbo/inspect_spec.rb")
    @script.write_spec("spec/core/tcejbo/inspect_spec.rb", "Object#inspect", true)
  end
end

describe MkSpec, "#create_file" do
  before :each do
    @script = MkSpec.new
    @script.stub!(:write_requires)
    @script.stub!(:write_spec)

    File.stub!(:exist?).and_return(false)
  end

  it "generates a file name based on the directory, class/module, and method" do
    File.should_receive(:join).with("spec/tcejbo", "inspect_spec.rb"
        ).and_return("spec/tcejbo/inspect_spec.rb")
    @script.create_file("spec/tcejbo", Object, "inspect")
  end

  it "does not call #write_requires if the spec file already exists" do
    File.should_receive(:exist?).and_return(true)
    @script.should_not_receive(:write_requires)
    @script.create_file("spec/tcejbo", Object, "inspect")
  end

  it "calls #write_requires if the spec file does not exist" do
    File.should_receive(:exist?).and_return(false)
    @script.should_receive(:write_requires).with(
        "spec/tcejbo", "spec/tcejbo/inspect_spec.rb")
    @script.create_file("spec/tcejbo", Object, "inspect")
  end

  it "calls #write_spec with the file, method name (Class#name) if instance is true" do
    @script.should_receive(:write_spec).with(
        "spec/tcejbo/inspect_spec.rb", "Object#inspect", false)
    @script.create_file("spec/tcejbo", Object, "inspect")
  end

  it "calls #write_spec with the file, method name (Class.name) if instance is false" do
    @script.should_receive(:write_spec).with(
        "spec/tcejbo/inspect_spec.rb", "Object.inspect", false)
    @script.create_file("spec/tcejbo", Object, "inspect", false)
  end
end

describe MkSpec, "#generate" do
  before :each do
    @script = MkSpec.new
    @script.stub!(:create_directory).and_return("subdir")
    @script.stub!(:create_file)
    @script.stub!(:map_constants).and_return([])

    @mod = mock("class/module", :null_object => true)
    @modules = [@mod]
  end

  it "creates a directory for each module" do
    @script.should_receive(:create_directory).with(@mod, "spec")
    @script.generate @modules, "spec"
  end

  it "gets a list of each module's methods" do
    @mod.should_receive(:methods).with(false).and_return([])
    @script.generate @modules, "spec"
  end

  it "calls #create_file for each module's methods" do
    @mod.should_receive(:methods).with(false).and_return(["method"])
    @script.should_receive(:create_file).with("subdir", @mod, "method", false)
    @script.generate @modules, "spec"
  end

  it "gets a list of each module's public_instance_methods" do
    @mod.should_receive(:public_instance_methods).with(false).and_return([])
    @script.generate @modules, "spec"
  end

  it "gets a list of each module's protected_instance_methods" do
    @mod.should_receive(:protected_instance_methods).with(false).and_return([])
    @script.generate @modules, "spec"
  end

  it "gets a list of each module's private_instance_methods" do
    @mod.should_receive(:private_instance_methods).with(false).and_return([])
    @script.generate @modules, "spec"
  end

  it "calls #create_file for each module's public, protected, and private instance methods" do
    @mod.stub!(:public_instance_methods).and_return(["public"])
    @mod.stub!(:protected_instance_methods).and_return(["protected"])
    @mod.stub!(:private_instance_methods).and_return(["private"])
    @script.should_receive(:create_file).with("subdir", @mod, "public")
    @script.should_receive(:create_file).with("subdir", @mod, "protected")
    @script.should_receive(:create_file).with("subdir", @mod, "private")
    @script.generate @modules, "spec"
  end
end

describe MkSpec, "#run" do
  before :each do
    @options = OptionParser.new
    OptionParser.stub!(:new).and_return(@options)
    @script = MkSpec.new
    @script.stub!(:generate)
    @script.stub!(:map_constants).and_return([Object])
  end

  it "loads files in the requires list" do
    @script.stub!(:require)
    @script.should_receive(:require).with("alib")
    @script.should_receive(:require).with("blib")
    @script.config[:requires] = ["alib", "blib"]
    @script.run
  end

  it "calls #map_constants with a list of constants" do
    @script.config[:base] = "spec"
    @script.config[:constants] = [Object]
    @script.should_receive(:map_constants).with([Object])
    @script.run
  end

  it "calls #map_constants with Object's constants if the list of constants is empty" do
    @script.config[:base] = "spec"
    @script.config[:constants] = []
    @script.should_receive(:map_constants).with(Object.constants)
    @script.run
  end

  it "calls #generate with the results of map_constants and the base directory" do
    @script.config[:base] = "spec"
    @script.should_receive(:map_constants).and_return(["Object"])
    @script.should_receive(:generate).with(["Object"], "spec")
    @script.run
  end
end

describe MkSpec, ".main" do
  before :each do
    @script = mock("MkSpec", :null_object => true)
    MkSpec.stub!(:new).and_return(@script)
  end

  it "creates an instance of MSpecScript" do
    MkSpec.should_receive(:new).and_return(@script)
    MkSpec.main
  end

  it "calls the #options method on the script" do
    @script.should_receive(:options)
    MkSpec.main
  end

  it "calls the #run method on the script" do
    @script.should_receive(:run)
    MkSpec.main
  end
end

