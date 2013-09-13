require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/module', __FILE__)

describe "Module#name" do
  describe "on named modules" do
    it "is set with nested names and it will not be changed by after definitions" do
      foo = TemporaryTopConstantForModuleName1 = Module.new

      begin
        bar = TemporaryTopConstantForModuleName1::Bar = Module.new
        baz = TemporaryTopConstantForModuleName1::Bar::Baz = Module.new
        foo.name.should == "TemporaryTopConstantForModuleName1"
        bar.name.should == "TemporaryTopConstantForModuleName1::Bar"
        baz.name.should == "TemporaryTopConstantForModuleName1::Bar::Baz"

        TemporaryTopConstantForModuleName1::Bar.send :remove_const, :Baz
        TemporaryTopConstantForModuleName1::Bar::Baz2 = baz
        baz.name.should == "TemporaryTopConstantForModuleName1::Bar::Baz"

        TemporaryTopConstantForModuleName1.send :remove_const, :Bar
        TemporaryTopConstantForModuleName1::Bar2 = bar
        bar.name.should == "TemporaryTopConstantForModuleName1::Bar"
        baz.name.should == "TemporaryTopConstantForModuleName1::Bar::Baz"
      ensure
        Object.send :remove_const, :TemporaryTopConstantForModuleName1
      end

      TemporaryTopConstantForModuleName2 = foo

      begin
        foo.name.should == "TemporaryTopConstantForModuleName1"
        bar.name.should == "TemporaryTopConstantForModuleName1::Bar"
        baz.name.should == "TemporaryTopConstantForModuleName1::Bar::Baz"
      ensure
        Object.send :remove_const, :TemporaryTopConstantForModuleName2
      end
    end
  end

  describe "on anonymous modulues" do
    it "will be set with nested names only once when top constant is defined" do
      foo, bar, baz = Module.new, Module.new, Module.new
      foo::Bar = bar
      foo::Bar::Baz = baz

      foo.name.to_s.should == ""
      bar.name.to_s.should == ""
      baz.name.to_s.should == ""

      TemporaryTopConstantForModuleName1 = foo

      begin
        foo.name.should == "TemporaryTopConstantForModuleName1"
        bar.name.should == "TemporaryTopConstantForModuleName1::Bar"
        baz.name.should == "TemporaryTopConstantForModuleName1::Bar::Baz"
      ensure
        Object.send :remove_const, :TemporaryTopConstantForModuleName1
      end

      TemporaryTopConstantForModuleName2 = foo

      begin
        foo.name.should == "TemporaryTopConstantForModuleName1"
        bar.name.should == "TemporaryTopConstantForModuleName1::Bar"
        baz.name.should == "TemporaryTopConstantForModuleName1::Bar::Baz"
      ensure
        Object.send :remove_const, :TemporaryTopConstantForModuleName2
      end
    end
  end

  ruby_version_is ""..."1.9" do
    it "is an empty string for an anonymous module" do
      Module.new.name.should == ""
    end

    it "is an empty string when assigning to a constant in an anonymous module" do
      m = Module.new
      m::N = Module.new
      m::N.name.should == ""
    end

    ruby_bug "http://bugs.ruby-lang.org/issues/6078", "2.0.0" do
      it "is an empty string for a nested module created with the module keyword" do
        m = Module.new
        module m::N; end
        m::N.name.should == ""
      end
    end
  end

  ruby_version_is "1.9" do
    it "is nil for an anonymous module" do
      Module.new.name.should be_nil
    end

    it "is nil when assigned to a constant in an anonymous module" do
      m = Module.new
      m::N = Module.new
      m::N.name.should be_nil
    end

    it "is nil for a nested module created with the module keyword" do
      m = Module.new
      module m::N; end
      m::N.name.should =~ /#<Module:0x[0-9a-f]+>::N/
    end
  end

  it "is set when opened with the module keyword" do
    ModuleSpecs.name.should == "ModuleSpecs"
  end

  it "is set when a nested module is opened with the module keyword" do
    ModuleSpecs::Anonymous.name.should == "ModuleSpecs::Anonymous"
  end

  it "is set when assigning to a constant" do
    m = Module.new
    ModuleSpecs::Anonymous::A = m
    m.name.should == "ModuleSpecs::Anonymous::A"
  end

  it "is not modified when assigning to a new constant after it has been accessed" do
    m = Module.new
    ModuleSpecs::Anonymous::B = m
    m.name.should == "ModuleSpecs::Anonymous::B"
    ModuleSpecs::Anonymous::C = m
    m.name.should == "ModuleSpecs::Anonymous::B"
  end

  ruby_version_is "1.9" do
    ruby_bug "http://bugs.ruby-lang.org/issues/6067", "2.0.0" do
      it "is set with a conditional assignment to a nested constant" do
        eval("ModuleSpecs::Anonymous::F ||= Module.new")
        ModuleSpecs::Anonymous::F.name.should == "ModuleSpecs::Anonymous::F"
      end
    end

    it "is set with a conditional assignment to a constant" do
      module ModuleSpecs::Anonymous
        D ||= Module.new
      end
      ModuleSpecs::Anonymous::D.name.should == "ModuleSpecs::Anonymous::D"
    end

    # http://redmine.ruby-lang.org/issues/show/1833
    it "preserves the encoding in which the class was defined" do
      require fixture(__FILE__, "name")
      ModuleSpecs::NameEncoding.new.name.encoding.should == Encoding::UTF_8
    end
  end

  it "is set when the anonymous outer module name is set" do
    m = Module.new
    m::N = Module.new
    ModuleSpecs::Anonymous::E = m
    m::N.name.should == "ModuleSpecs::Anonymous::E::N"
  end
end
