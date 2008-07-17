require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'
require File.dirname(__FILE__) + '/shared/each'

describe "StringIO#each when passed a separator" do
  it_behaves_like :stringio_each_separator, :each_line
end

describe "StringIO#each when passed no arguments" do
  it_behaves_like :stringio_each_no_arguments, :each_line
end

describe "StringIO#each when self is not readable" do
  it_behaves_like :stringio_each_not_readable, :each_line
end
