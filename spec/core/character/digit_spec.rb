# -*- encoding: utf-8 -*-
require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/common', __FILE__)

with_feature :encoding do
  describe "Rubinius::Character#digit?" do
    it "returns true for a numerical character" do
      char = new_character "1"
      char.digit?.should be_true
    end

    it "returns false for a non-numerical character" do
      char = new_character "a"
      char.digit?.should be_false
    end
  end
end
