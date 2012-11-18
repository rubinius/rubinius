# -*- encoding: utf-8 -*-
require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/common', __FILE__)

with_feature :encoding do
  describe "Rubinius::Character#printable?" do
    it "returns true for a printable character" do
      char = new_character "ー"
      char.printable?.should be_true
    end

    it "returns false for a non-printable character" do
      char = new_character "\b"
      char.printable?.should be_false
    end
  end
end
