require File.dirname(__FILE__) + '/../../spec_helper'

ruby_version_is "1.9" do
  describe "Encoding.locale_charmap" do
    it "returns a String" do
      Encoding.locale_charmap.should be_an_instance_of(String)
    end

    # FIXME: Get this working on Windows
    platform_is :os => [:darwin, :linux] do
      it "returns a value based on the LANG environment variable" do
        old_lang = ENV['LANG']
        ENV['LANG'] = 'C'
        ruby_exe("print Encoding.locale_charmap").should == 'ANSI_X3.4-1968'
        ENV['LANG'] = old_lang
      end

      it "is unaffected by assigning to ENV['LANG'] in the same process" do
        old_charmap = Encoding.locale_charmap
        old_lang = ENV['LANG']
        ENV['LANG'] = 'C'
        Encoding.locale_charmap.should == old_charmap
        ENV['LANG'] = old_lang
      end
    end
  end
end