require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

ruby_version_is "1.9" do
  describe "Kernel#initialize_dup" do
    it "calls #initialize_copy" do
      ScratchPad.clear

      obj = KernelSpecs::Duplicate.new(1, :a)
      other = KernelSpecs::Duplicate.new(2, :b)

      obj.initialize_dup(other)

      ScratchPad.recorded.should == obj.object_id
    end
  end
end
