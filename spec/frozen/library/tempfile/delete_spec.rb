require File.dirname(__FILE__) + '/../../spec_helper'
require 'tempfile'
require File.dirname(__FILE__) + "/shared/unlink"

describe "Tempfile#delete" do
  it_behaves_like :tempfile_unlink, :delete
end
