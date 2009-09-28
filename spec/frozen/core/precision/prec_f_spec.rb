require File.dirname(__FILE__) + '/../../spec_helper'

ruby_version_is ""..."1.9.1" do
  describe "Precision#prec_f" do
    it "converts a Integer to Float when called on an Integer" do
      1.prec_f.should==1.0
    end

    it "returns the same Float when called on a Float" do
      1.9.prec_f.should==1.9
    end
  end
end
