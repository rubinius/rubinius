require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

describe :kernel_chomp, :shared => true do
  it "removes the final newline of $_" do
    KernelSpecs.chomp("abc\n", @method).should == "abc"
  end

  it "removes the final carriage return of $_" do
    KernelSpecs.chomp("abc\r", @method).should == "abc"
  end

  it "removes the final carriage return, newline of $_" do
    KernelSpecs.chomp("abc\r\n", @method).should == "abc"
  end

  it "removes only the last newline of $_" do
    KernelSpecs.chomp("abc\n\n", @method).should == "abc\n"
  end

  it "removes the value of $/ from the end of $_" do
    KernelSpecs.chomp("abcde", @method, "cde").should == "ab"
  end
end

describe "Kernel.chomp" do
  it_behaves_like :kernel_chomp, "Kernel.chomp"
end

ruby_version_is ""..."1.9" do
  describe "Kernel.chomp!" do
    it_behaves_like :kernel_chomp, "Kernel.chomp!"
  end
end

describe "Kernel#chomp" do
  it_behaves_like :kernel_chomp, "chomp"

  it "is a private method" do
    KernelSpecs.has_private_method(:chomp).should be_true
  end
end

ruby_version_is ""..."1.9" do
  describe "Kernel#chomp!" do
    it_behaves_like :kernel_chomp, "chomp!"

    it "is a private method" do
      KernelSpecs.has_private_method(:chomp!).should be_true
    end
  end
end
