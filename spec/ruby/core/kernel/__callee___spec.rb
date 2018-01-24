require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)
require File.expand_path('../shared/__method__', __FILE__)

describe "Kernel.__callee__" do
  it_behaves_like(:kernel___method__, :__callee__)
end

describe "Kernel.__callee__" do
  it "returns the name of the alias when a method is aliased" do
    class KernelSpecs::Callee
      def m
        __callee__
      end

      alias mm m
    end

    KernelSpecs::Callee.new.mm.should == :mm
  end
end
