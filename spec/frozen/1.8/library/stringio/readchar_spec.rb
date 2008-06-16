require File.dirname(__FILE__) + '/../../spec_helper'
require 'stringio'
require File.dirname(__FILE__) + "/shared/readchar"

describe "StringIO#readchar" do
  it_behaves_like :stringio_readchar, :readchar
end
