require File.dirname(__FILE__) + '/../../spec_helper'
require 'tempfile'
require File.dirname(__FILE__) + "/shared/length"

describe "Tempfile#length" do
  it_behaves_like :tempfile_length, :length
end
