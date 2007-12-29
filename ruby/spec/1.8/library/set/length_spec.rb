require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/length'
require 'set'

describe "Set#length" do
  it_behaves_like :set_length, :length
end
