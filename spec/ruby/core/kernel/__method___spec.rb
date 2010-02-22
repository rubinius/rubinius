require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)
require File.expand_path('../shared/__method__', __FILE__)

describe "Kernel.__method__" do
  ruby_version_is '1.8.7' do
    it_behaves_like(:kernel___method__, :__method__)
  end
end
