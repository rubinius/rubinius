require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

ruby_version_is "1.9" do
  describe "Module#prepend" do
    it "prepends module do class" do
      ModuleSpecs::ClassToPrepend.new.m.should == [:m, :c]
    end

    it "prepends modules in proper sequence" do
      obj = ModuleSpecs::PrependModules::C0.new
      obj.m1.should == [:M1,:C0,:M0]

      obj = ModuleSpecs::PrependModules::C1.new
      obj.m1.should == [:M2,:M3,:C1,:M4,:M1,:C0,:M0]
    end

    it "does not prepend twice one module" do
      m = Module.new

      lambda {
        @klass = Class.new do
          2.times { prepend m }
        end
      }.should_not raise_error(ArgumentError)

      @klass.ancestors.count(m).should == 1
    end

    it "prepends instance methods" do
      Object.instance_methods.should == Class.new {prepend Module.new}.instance_methods
    end

    it "prepends singleton methods" do
      o = Object.new
      o.singleton_class.class_eval {prepend Module.new}
      o.singleton_methods.should == []
    end

    it "raises NameError in case of remove method" do
      lambda {
        Class.new do
          prepend Module.new {def foo; end}
          remove_method(:foo)
        end
      }.should raise_error(NameError)
    end

    it "returns proper class ancestors" do
      m = labeled_module("m")
      c = labeled_class("c") {prepend m}
      c.ancestors[0, 2].should == [m, c]

      c2 = labeled_class("c2", c)
      anc = c2.ancestors
      anc[0..anc.index(Object)].should == [c2, m, c, Object]
    end

    it "returns proper prepend module ancestors" do
      m0 = labeled_module("m0") {def x; [:m0, *super] end}
      m1 = labeled_module("m1") {def x; [:m1, *super] end; prepend m0}
      m2 = labeled_module("m2") {def x; [:m2, *super] end; prepend m1}
      c0 = labeled_class("c0") {def x; [:c0] end}
      c1 = labeled_class("c1") {def x; [:c1] end; prepend m2}
      c2 = labeled_class("c2", c0) {def x; [:c2, *super] end; include m2}

      m1.ancestors.should == [m0, m1]

      c1.ancestors[0, 4].should == [m0, m1, m2, c1]
      m2.ancestors.should == [m0, m1, m2]
      c1.new.x.should == [:m0, :m1, :m2, :c1]
      c2.ancestors[0, 5].should == [c2, m0, m1, m2, c0]
      c2.new.x.should == [:c2, :m0, :m1, :m2, :c0]
    end

    it "prepends instance methods" do
      Class.new{ prepend Module.new; def m1; end }.instance_methods(false).should == [:m1]
      Class.new(Class.new{def m2;end}){ prepend Module.new; def m1; end }.instance_methods(false).should == [:m1]
    end

    it "updates ancestors after prepend" do
      m   = Module.new
      m1  = Module.new
      c   = Class.new { prepend m }
      c1  = Class.new(c)

      c1.ancestors.should include(m)
      c1.ancestors.should_not include(m1)

      c.send(:prepend, m1)
      c1.ancestors.should include(m1)
    end
  end
end
