require File.dirname(__FILE__) + '/../../spec_helper'
require 'set'
require File.dirname(__FILE__) + '/shared/delete'

describe "Set#delete" do
  it_behaves_like :set_delete, Set
end

describe "Set#delete?" do
  it_behaves_like :set_delete_p, Set
end
