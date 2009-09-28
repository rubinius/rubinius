require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/../../../shared/complex/numeric/polar'

ruby_version_is ""..."1.9" do

  require 'complex'
  require 'rational'

  describe "Numeric#polar" do
    it_behaves_like(:numeric_polar, :polar)
  end
end
