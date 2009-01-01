require File.dirname(__FILE__) + '/../../spec_helper'
require 'stringio'
require File.dirname(__FILE__) + '/shared/getc'

describe "StringIO#getc" do
  it_behaves_like :stringio_getc, :getc
end

describe "StringIO#getc when self is not readable" do
  it_behaves_like :stringio_getc_not_readable, :getc
end
