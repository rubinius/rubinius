require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

describe "Kernel.chop" do
  it "removes the final character of $_" do
    KernelSpecs.chop("abc", "Kernel.chop").should == "ab"
  end
end

ruby_version_is ""..."1.9" do
  describe ".chop!" do
    it "removes the final character of $_" do
      KernelSpecs.chop("abc", "Kernel.chop!").should == "ab"
    end
  end
end

describe "#chop" do
  it "is a private method" do
    KernelSpecs.has_private_method(:chop).should be_true
  end

  it "removes the final character of $_" do
    KernelSpecs.chop("abc", "chop").should == "ab"
  end
end

ruby_version_is ""..."1.9" do
  describe "#chop!" do
    it "is a private method" do
      KernelSpecs.has_private_method(:chop!).should be_true
    end

    it "removes the final character of $_" do
      KernelSpecs.chop("abc", "chop!").should == "ab"
    end
  end
end
