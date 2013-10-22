# -*- encoding: utf-8 -*-
require File.expand_path('../../../spec_helper', __FILE__)

describe "MatchData" do
  ruby_version_is "1.9" do
    it "exposes the named captures of the regexp" do
      match_data = /\A(?<intro>\w+) (?<outro>.*)\Z/.match("foo bar baz")

      match_data[:intro].should == "foo"
      match_data['intro'].should == "foo"

      match_data[:outro].should == "bar baz"
      match_data['outro'].should == "bar baz"
    end

    it "exposes the matching version of multiple named captures" do
      regexp = /(?:
          A(?<word>\w+)
        |
          B(?<word>\w+)
      )/x
      match_data_a = regexp.match("Afoo")
      match_data_b = regexp.match("Bfoo")

      match_data_a[:word].should == "foo"
      match_data_b[:word].should == "foo"

      match_data_a['word'].should == "foo"
      match_data_b['word'].should == "foo"
    end

    it "exposes the last match when multiple versions of the same named capture matches" do
      match_data = /(?<word>foo)(?<word>bar)/.match("foobar")
      match_data[:word].should == "bar"
      match_data['word'].should == "bar"
    end

    it "returns nil on non-matching named captures" do
      regexp = /(?<foo>foo )?(?<bar>bar)/
      full_match = regexp.match("foo bar")
      partial_match = regexp.match("bar")

      full_match[:foo].should == "foo "
      partial_match[:foo].should == nil

      full_match['foo'].should == "foo "
      partial_match['foo'].should == nil
    end

    it "raises an IndexError when accessing an undefined capture" do
      match_data = /(?<foo>.)/.match("x")
      lambda { match_data[:bar] }.should raise_error(IndexError, /bar/)
      lambda { match_data['bar'] }.should raise_error(IndexError, /bar/)
    end
  end
end
