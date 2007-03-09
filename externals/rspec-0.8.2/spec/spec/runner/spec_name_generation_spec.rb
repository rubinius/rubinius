require File.dirname(__FILE__) + '/../../spec_helper.rb'

module Spec
  module Runner
    module SpecNameSpecHelper
      def run_spec
        @spec.run(reporter, nil, nil, nil, ::Spec::Runner::ExecutionContext.new(nil))
      end
      
      def reporter
        @reporter ||= mock("reporter")
      end
      
      def spec_class
        @spec_class ||= generate_spec_class
      end
      
      def generate_spec_class
        callback_container = Callback::CallbackContainer.new
        spec_class = Specification.dup
        spec_class.stub!(:callbacks).and_return {callback_container}
        spec_class
      end
    end
    
    # context "Specification with given name" do
    #   include SpecNameSpecHelper
    #   setup do
    #     @spec = spec_class.new("given name")
    #   end
    #   
    #   specify "should start spec with given name" do
    #     reporter.stub!(:spec_finished)
    #     reporter.should_receive(:spec_started).with("given name")
    #     run_spec
    #   end
    #   
    #   specify "should finish spec with given name" do
    #     reporter.stub!(:spec_started)
    #     reporter.should_receive(:spec_finished).with("given name", :anything, :anything)
    #     run_spec
    #   end
    # end
    # 
    # context "Specification with given name and error" do
    #   include SpecNameSpecHelper
    #   setup do
    #     @spec = spec_class.new("given name") { raise }
    #   end
    #   
    #   specify "should start spec with given name" do
    #     reporter.stub!(:spec_finished)
    #     reporter.should_receive(:spec_started).with("given name")
    #     run_spec
    #   end
    #   
    #   specify "should finish spec with given name" do
    #     reporter.stub!(:spec_started)
    #     reporter.should_receive(:spec_finished).with("given name", :anything, :anything)
    #     run_spec
    #   end
    # end
    
    context "Specification with generated name" do
      include SpecNameSpecHelper
      setup do
        @spec = spec_class.new(:__generate_description) { 5.should == 5 }
      end
      
      # specify "should start spec with given name" do
      #   reporter.stub!(:spec_finished)
      #   reporter.should_receive(:spec_started).with("NAME NOT GENERATED")
      #   run_spec
      # end
      
      specify "should finish spec with given name" do
        reporter.stub!(:spec_started)
        reporter.should_receive(:spec_finished).with("should == 5", :anything, :anything)
        run_spec
      end
    end
    
    # context "Specification with generated name and error" do
    #   include SpecNameSpecHelper
    #   setup do
    #     @spec = spec_class.new(:__generate_description) { raise }
    #   end
    #   
    #   specify "should start spec with given name" do
    #     reporter.stub!(:spec_finished)
    #     reporter.should_receive(:spec_started).with("NAME NOT GENERATED")
    #     run_spec
    #   end
    #   
    #   specify "should finish spec with given name" do
    #     reporter.stub!(:spec_started)
    #     reporter.should_receive(:spec_finished).with("NAME NOT GENERATED", :anything, :anything)
    #     run_spec
    #   end
    # end
  end
end