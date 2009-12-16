require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'
require File.dirname(__FILE__) + '/shared/length'

describe "StringIO#length" do
  it_behaves_like :stringio_length, :length
end
