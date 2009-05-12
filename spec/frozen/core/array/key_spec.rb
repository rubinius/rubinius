require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'
require File.dirname(__FILE__) + '/shared/index'

describe "Array#key" do
  ruby_version_is '1.9' do
    it_behaves_like :array_index, :key
  end
end
