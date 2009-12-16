require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'
require File.dirname(__FILE__) + '/shared/__method__'

describe "Kernel.__method__" do
  ruby_version_is '1.8.7' do
    it_behaves_like(:kernel___method__, :__method__)
  end
end
