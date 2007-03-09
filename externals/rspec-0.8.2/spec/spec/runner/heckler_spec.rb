require File.dirname(__FILE__) + '/../../spec_helper.rb'
unless ['i386-mswin32', 'java'].index(PLATFORM)
  require 'spec/runner/heckle_runner'

  context "Heckler" do
    specify "should run context_runner on tests_pass?" do
      context_runner = mock("context_runner")
      context_runner.should_receive(:run)
      heckler = Spec::Runner::Heckler.new('Array', 'push', context_runner)

      heckler.tests_pass?
    end
  end
end