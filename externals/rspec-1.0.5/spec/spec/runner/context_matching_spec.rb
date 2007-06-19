require File.dirname(__FILE__) + '/../../spec_helper.rb'

module Spec
  module DSL
    describe "ContextMatching" do

      before(:each) do
        @formatter = Spec::Mocks::Mock.new("formatter")
        @behaviour = Behaviour.new("context") {}
      end

      it "run all specs when spec is not specified" do
        @behaviour.it("spec1") {}
        @behaviour.it("spec2") {}
        @behaviour.retain_examples_matching!(["context"])
        @behaviour.number_of_examples.should == 2
      end

      it "should only run specified examples when specified" do
        @behaviour.it("spec1") {}
        @behaviour.it("spec2") {}
        @behaviour.retain_examples_matching!(["context spec1"])
        @behaviour.number_of_examples.should == 1
      end
    end
  end
end
