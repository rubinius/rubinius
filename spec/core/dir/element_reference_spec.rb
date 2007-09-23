require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/common'
require File.dirname(__FILE__) + '/glob_spec'

describe "Dir.[]" do
  it_behaves_like @dir_glob, :[]
end
