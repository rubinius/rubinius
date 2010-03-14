require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)
require File.expand_path('../shared/exit', __FILE__)

ruby_version_is ""..."1.9.1" do
  describe "Thread#exit" do
    it_behaves_like :thread_exit, :exit
  end
end

describe "Thread#exit!" do
  it "needs to be reviewed for spec completeness"
end

describe "Thread.exit" do
  it "needs to be reviewed for spec completeness"
end
