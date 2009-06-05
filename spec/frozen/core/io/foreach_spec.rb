# encoding: utf-8
require File.dirname(__FILE__) + '/../../spec_helper'

describe "IO::foreach" do
  before :all do
    @file = File.dirname(__FILE__) + '/fixtures/readlines.txt'
    @content = ["Voici la ligne une.\n", "Qui \303\250 la linea due.\n",
                "Aqu\303\255 est\303\241 la l\303\255nea tres.\n", "Ist hier Linie vier.\n",
                "Est\303\241 aqui a linha cinco.\n", "Here is line six.\n"]
    @content_with_r = ["Voici la ligne une.\nQui \303\250 la linea due.\nAqu\303\255 est\303\241 la l\303\255nea tr",
                     "es.\nIst hier", " Linie vier",
                     ".\nEst\303\241 aqui a linha cinco.\nHer","e is line six.\n"]
  end
  after :all do
    @file = nil
    @content = nil
  end

  it "yields a sequence of Strings that were separated by $/" do
    lines = []
    IO::foreach(@file) do |line|
      lines << line
    end
    lines.should == @content
  end

  it "yields a sequence of Strings that were separated by r" do
    lines = []
    IO::foreach(File.dirname(__FILE__) + '/fixtures/readlines.txt','r') do |line|
      lines << line
    end
    lines.should == @content_with_r
  end

  it "yields a single string with entire content when the separator is nil" do
    lines = []
    IO::foreach(@file, nil) do |line|
      lines << line
    end
    lines.should == [@content.join]
  end

  it "yields a sequence of paragraphs when the separator is an empty string" do
    lines = []
    IO::foreach(File.dirname(__FILE__) + '/fixtures/gets.txt', "") do |line|
      lines << line
    end
    lines.should == [
      "Voici la ligne une.\nQui \303\250 la linea due.\n\n",
      "Aqu\303\255 est\303\241 la l\303\255nea tres.\nIst hier Linie vier.\n\n",
      "Est\303\241 aqui a linha cinco.\nHere is line six.\n"]
  end

  it "updates $. with each yield" do
    count = 1
    IO::foreach(@file, nil) do
      $..should == count
      count += 1
    end

    count = 1
    IO::foreach(@file, "") do
      $..should == count
      count += 1
    end

    count = 1
    IO::foreach(@file) do
      $..should == count
      count += 1
    end

    count = 1
    IO::foreach(@file, "la") do
      $..should == count
      count += 1
    end
  end

  it "can handle non-ASCII data as separator" do
    lines = []
    IO::foreach(@file, "\303\250") do |line|
      lines << line
    end
    lines.should == ["Voici la ligne une.\nQui \303\250",
      " la linea due.\nAqu\303\255 est\303\241 la l\303\255nea tres.\nIst hier Linie vier.\nEst\303\241 aqui a linha cinco.\nHere is line six.\n"]
  end

  it "raises TypeError if the first parameter is nil" do
    lambda {
      IO::foreach(nil) {}
    }.should raise_error(TypeError)
  end

  it "converts first parameter to string and uses as file name" do
    (obj = mock('readlines.txt')).should_receive(:to_str).and_return(@file)
    lines = []
    IO::foreach(obj) do |line|
      lines << line
    end
    lines.should == @content
  end

  ruby_version_is "" ... "1.9" do
    it "converts second parameter to string and uses as separator, with a to_str calling for the IO" do
      (obj = mock('r')).should_receive(:to_str).once.and_return('r')
      lines = []
      IO::foreach(@file, obj) do |line|
        lines << line
      end
      lines.should == @content_with_r
    end
  end

  ruby_version_is "1.9" do
    it "converts second parameter to string and uses as separator, with each to_str calling for each lines of the IOs" do
      num_lines = 6 # File.read(@file).lines.to_a.size #=> 6
      (obj = mock('r')).should_receive(:to_str).exactly(num_lines).times.and_return('r')
      lines = []
      IO::foreach(@file, obj) do |line|
        lines << line
      end
      lines.should == @content_with_r
    end
  end
end
