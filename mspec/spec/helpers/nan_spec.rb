require File.dirname(__FILE__) + '/../spec_helper'
require 'mspec/helpers/nan'

describe Object, "#nan_value" do
  it "returns NaN" do
    nan_value.nan?.should be_true
  end
end
