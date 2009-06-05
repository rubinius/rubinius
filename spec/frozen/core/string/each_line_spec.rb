require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'
require File.dirname(__FILE__) + '/shared/each_line'

describe "String#each_line" do
  it_behaves_like(:string_each_line, :each_line)
end
