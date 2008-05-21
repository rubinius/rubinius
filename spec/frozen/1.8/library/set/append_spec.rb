require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/add'
require 'set'

describe "Set#<<" do
  it_behaves_like :set_add, :<<
end
