require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/methods', __FILE__)
require File.expand_path('../shared/now', __FILE__)
require File.expand_path('../shared/local', __FILE__)
require File.expand_path('../shared/time_params', __FILE__)

describe "Time.new" do
  it_behaves_like(:time_now, :new)
end

ruby_version_is "1.9" do
  describe "Time.new" do
    it_behaves_like(:time_local, :new)
    it_behaves_like(:time_params, :new)
  end

  describe "Time.new with a utc_offset argument" do
    it "needs to be reviewed for spec completeness"
  end
end
