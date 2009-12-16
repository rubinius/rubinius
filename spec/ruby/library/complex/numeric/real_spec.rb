require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/../../../shared/complex/numeric/real'

ruby_version_is ""..."1.9" do

  require 'complex'
  require 'rational'

  describe "Numeric#real" do
    it_behaves_like(:numeric_real, :real)
  end
end
