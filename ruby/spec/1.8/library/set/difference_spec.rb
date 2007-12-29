require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/difference'
require 'set'

describe "Set#difference" do
  it_behaves_like :set_difference, :difference
end
