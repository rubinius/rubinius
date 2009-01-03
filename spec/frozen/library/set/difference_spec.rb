require File.dirname(__FILE__) + '/../../spec_helper'
require 'set'
require File.dirname(__FILE__) + '/shared/difference'

describe "Set#difference" do
  it_behaves_like :set_difference, :difference
end
