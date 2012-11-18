# -*- encoding: utf-8 -*-
require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/common', __FILE__)

with_feature :encoding do
  describe "Rubinius::Character#punctuation?" do
    it "returns true for a punctuation character" do
      char = new_character "."
      char.punctuation?.should be_true
    end

    it "returns false for a non-punctuation character" do
      char = new_character "a"
      char.punctuation?.should be_false
    end
  end
end
