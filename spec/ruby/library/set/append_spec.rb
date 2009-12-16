require File.dirname(__FILE__) + '/../../spec_helper'
require 'set'
require File.dirname(__FILE__) + '/shared/add'

describe "Set#<<" do
  it_behaves_like :set_add, :<<
end
