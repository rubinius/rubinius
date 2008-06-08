require File.dirname(__FILE__) + '/../../spec_helper'
require 'set'
require File.dirname(__FILE__) + '/shared/clear'

describe "Set#clear" do
  it_behaves_like :set_clear, Set
end
