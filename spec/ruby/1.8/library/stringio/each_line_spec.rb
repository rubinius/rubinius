require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'
require File.dirname(__FILE__) + '/shared/each'

describe "StringIO#each_line" do
  it_behaves_like :stringio_each, :each_line
end
