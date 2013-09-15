require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../shared/equal', __FILE__)

describe "Proc#eql?" do
  ruby_version_is "1.9"..."2.0" do
    it_behaves_like(:proc_equal, :eql?)
  end

  ruby_version_is "2.0" do
    it_behaves_like(:proc_equal_undefined, :eql?)
  end
end
