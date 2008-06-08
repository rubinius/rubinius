require File.dirname(__FILE__) + '/../../spec_helper'
require 'set'
require File.dirname(__FILE__) + '/shared/classify'

describe "Set#classify" do
  it_behaves_like :set_classify, Set
end
