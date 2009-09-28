require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/call'

ruby_version_is "1.9" do
  describe "Proc#yield" do
    it_behaves_like :proc_call, :yield
  end

  describe "Proc#yield on a Proc created with Proc.new" do
    it_behaves_like :proc_call_on_proc_new, :yield
  end

  describe "Proc#yield on a Proc created with Kernel#lambda or Kernel#proc" do
    it_behaves_like :proc_call_on_proc_or_lambda, :yield
  end
end
