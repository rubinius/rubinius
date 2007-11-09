require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/each.rb'

describe "IO#each" do
    it_behaves_like(:io_each, :each)
end
