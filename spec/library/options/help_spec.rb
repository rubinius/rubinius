require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../shared/help', __FILE__)

describe "Options#help" do
  it_behaves_like :lib_options_usage_help, :help
end
