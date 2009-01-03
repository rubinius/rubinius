require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/common'
require File.dirname(__FILE__) + '/shared/glob'

describe "Dir.[]" do
  it_behaves_like :dir_glob, :[]
end

describe "Dir.[]" do
  it_behaves_like :dir_glob_recursive, :[]
end
