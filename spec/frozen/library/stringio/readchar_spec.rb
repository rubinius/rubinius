require File.dirname(__FILE__) + '/../../spec_helper'
require 'stringio'
require File.dirname(__FILE__) + "/shared/readchar"

describe "StringIO#readchar" do
  it_behaves_like :stringio_readchar, :readchar
end

describe "StringIO#readchar when self is not readable" do
  it_behaves_like :stringio_readchar_not_readable, :readchar
end
