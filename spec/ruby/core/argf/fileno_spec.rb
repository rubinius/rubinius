require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/fileno'

describe "ARGF.fileno" do
  it_behaves_like :argf_fileno, :fileno
end
