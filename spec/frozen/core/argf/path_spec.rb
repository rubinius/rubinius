require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/filename'

describe "ARGF.path" do
  it_behaves_like :argf_filename, :path
end
