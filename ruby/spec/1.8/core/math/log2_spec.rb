require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'
require File.dirname(__FILE__) + '/shared/log2'

platform_is :version => '1.9.0' do
  describe "Math.log2" do
    it_behaves_like(:math_log2, :log2)
  end
end

extended_on :rubinius do
  describe "Math.log2" do
    it_behaves_like(:math_log2, :log2)
  end
end
  