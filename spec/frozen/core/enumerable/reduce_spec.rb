require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'
require File.dirname(__FILE__) + '/shared/inject'

describe "Enumerable#reduce" do
  ruby_version_is '1.8.7' do
    it_behaves_like :enumerable_inject, :reduce
  end
end