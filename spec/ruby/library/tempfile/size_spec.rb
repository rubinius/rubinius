require File.expand_path('../../../spec_helper', __FILE__)
require 'tempfile'
require File.expand_path('../shared/length', __FILE__)

describe "Tempfile#size" do
  it_behaves_like :tempfile_length, :size
end
