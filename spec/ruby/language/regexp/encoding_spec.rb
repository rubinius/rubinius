# -*- encoding: US-ASCII -*-
require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../../fixtures/classes', __FILE__)

describe "Regexps with encoding modifiers" do
  it "supports /e (EUC encoding)" do
    match = /./e.match("\303\251")
    match.to_a.should == ["\303\251"]
  end

  it "supports /e (EUC encoding) with interpolation" do
    match = /#{/./}/e.match("\303\251")
    match.to_a.should == ["\303\251"]
  end

  it "supports /e (EUC encoding) with interpolation and /o" do
    match = /#{/./}/e.match("\303\251")
    match.to_a.should == ["\303\251"]
  end

  it "supports /n (Normal encoding)" do
    /./n.match("\303\251").to_a.should == ["\303"]
  end

  it "supports /n (Normal encoding) with interpolation" do
    /#{/./}/n.match("\303\251").to_a.should == ["\303"]
  end

  it "supports /n (Normal encoding) with interpolation and /o" do
    /#{/./}/no.match("\303\251").to_a.should == ["\303"]
  end

  it "supports /s (SJIS encoding)" do
    /./s.match("\303\251").to_a.should == ["\303"]
  end

  it "supports /s (SJIS encoding) with interpolation" do
    /#{/./}/s.match("\303\251").to_a.should == ["\303"]
  end

  it "supports /s (SJIS encoding) with interpolation and /o" do
    /#{/./}/so.match("\303\251").to_a.should == ["\303"]
  end

  it "supports /u (UTF8 encoding)" do
    /./u.match("\303\251").to_a.should == ["\303\251"]
  end

  it "supports /u (UTF8 encoding) with interpolation" do
    /#{/./}/u.match("\303\251").to_a.should == ["\303\251"]
  end

  it "supports /u (UTF8 encoding) with interpolation and /o" do
    /#{/./}/uo.match("\303\251").to_a.should == ["\303\251"]
  end

  it "selects last of multiple encoding specifiers" do
    /foo/ensuensuens.should == /foo/s
  end
end
