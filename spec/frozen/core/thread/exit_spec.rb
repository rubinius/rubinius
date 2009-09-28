require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'
require File.dirname(__FILE__) + '/shared/exit'

ruby_version_is ""..."1.9.1" do
  describe "Thread#exit" do
    it_behaves_like :thread_exit, :exit
  end
end
