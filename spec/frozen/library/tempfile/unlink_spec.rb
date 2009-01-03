require File.dirname(__FILE__) + '/../../spec_helper'
require 'tempfile'
require File.dirname(__FILE__) + "/shared/unlink"

describe "Tempfile#unlink" do
  it_behaves_like :tempfile_unlink, :unlink
end
