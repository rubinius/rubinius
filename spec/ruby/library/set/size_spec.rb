require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/length'
require 'set'

describe "Set#size" do
  it_behaves_like :set_length, :size
end
