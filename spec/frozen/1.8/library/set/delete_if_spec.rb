require File.dirname(__FILE__) + '/../../spec_helper'
require 'set'
require File.dirname(__FILE__) + '/shared/delete_if'

describe "Set#delete_if" do
  it_behaves_like :set_delete_if, Set
end
