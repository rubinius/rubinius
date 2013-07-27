require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

ruby_version_is "2.0" do
  describe "Module#prepend_features" do
    it "gets called when self is included in another module/class" do
      ScratchPad.record []

      m = Module.new do
        def self.prepend_features(mod)
          ScratchPad << mod
        end
      end

      c = Class.new do
        prepend m
      end

      ScratchPad.recorded.should == [c]
    end

    it "raises an ArgumentError on a cyclic prepend" do
      lambda {
        ModuleSpecs::CyclicPrepend.send(:prepend_features, ModuleSpecs::CyclicPrepend)
      }.should raise_error(ArgumentError)
    end
  end
end
