# encoding: utf-8
require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "IO#readlines when passed no arguments" do
  before(:each) do
    @io = File.open(File.dirname(__FILE__) + '/fixtures/readlines.txt')
  end

  after(:each) do
    @io.close
  end

  it "returns an Array containing lines based on $/" do
    begin
      old_sep, $/ = $/, " "
      @io.readlines.should == ["Voici ", "la ", "ligne ", "une.\nQui ", "\303\250 ",
        "la ", "linea ", "due.\nAqu\303\255 ", "est\303\241 ", "la ", "l\303\255nea ",
        "tres.\nIst ", "hier ", "Linie ", "vier.\nEst\303\241 ", "aqui ", "a ",
        "linha ", "cinco.\nHere ", "is ", "line ", "six.\n"]
    ensure
      $/ = old_sep
    end
  end

  it "updates self's position" do
    @io.readlines
    @io.pos.should eql(134)
  end

  it "updates self's lineno based on the number of lines read" do
    @io.readlines
    @io.lineno.should eql(6)
  end

  it "does not change $_" do
    $_ = "test"
    @io.readlines(">")
    $_.should == "test"
  end

  it "returns an empty Array when self is at the end" do
    @io.pos = 134
    @io.readlines.should == []
  end
end

describe "IO#readlines when passed [separator]" do
  before(:each) do
    @io = File.open(File.dirname(__FILE__) + '/fixtures/readlines.txt')
  end

  after(:each) do
    @io.close
  end

  it "returns an Array containing lines based on the passed separator" do
    @io.readlines('r').should == [
      "Voici la ligne une.\nQui \303\250 la linea due.\nAqu\303\255 est\303\241 la l\303\255nea tr",
      "es.\nIst hier",
      " Linie vier",
      ".\nEst\303\241 aqui a linha cinco.\nHer",
      "e is line six.\n"]
  end

  it "returns an empty Array when self is at the end" do
    @io.pos = 134
    @io.readlines.should == []
  end

  it "updates self's lineno based on the number of lines read" do
    @io.readlines('r')
    @io.lineno.should eql(5)
  end

  it "updates self's position based on the number of characters read" do
    @io.readlines("r")
    @io.pos.should eql(134)
  end

  it "does not change $_" do
    $_ = "test"
    @io.readlines("r")
    $_.should == "test"
  end

  it "returns an Array containing all paragraphs when the passed separator is an empty String" do
    File.open(File.dirname(__FILE__) + '/fixtures/paragraphs.txt') do |io|
      io.readlines("").should == ["This is\n\n", "an example\n\n", "of paragraphs."]
    end
  end

  it "returns the remaining content as one line starting at the current position when passed nil" do
    @io.pos = 5
    @io.readlines(nil).should == [" la ligne une.\nQui \303\250 la linea due.\n" + 
      "Aqu\303\255 est\303\241 la l\303\255nea tres.\n" +
      "Ist hier Linie vier.\nEst\303\241 aqui a linha cinco.\nHere is line six.\n"]
  end

  it "tries to convert the passed separator to a String using #to_str" do
    obj = mock('to_str')
    obj.stub!(:to_str).and_return("r")
    @io.readlines(obj).should == [
      "Voici la ligne une.\nQui \303\250 la linea due.\nAqu\303\255 est\303\241 la l\303\255nea tr",
      "es.\nIst hier",
      " Linie vier",
      ".\nEst\303\241 aqui a linha cinco.\nHer",
      "e is line six.\n"]
  end
end

describe "IO#readlines when in write-only mode" do
  it "raises an IOError" do
    path = tmp("write_only_specs")
    File.open(path, 'a') do |io|
      lambda { io.readlines }.should raise_error(IOError)
    end

    File.open(path) do |io|
      io.close_read
      lambda { io.readlines }.should raise_error(IOError)
    end
    File.unlink(path) if File.exists?(path)
  end
end

describe "IO.readlines when passed [file_name]" do
  before(:each) do
    @file = File.dirname(__FILE__) + '/fixtures/readlines.txt'
  end

  it "returns an Array containing lines of file_name based on $/" do
    begin
      old_sep, $/ = $/, " "
      IO.readlines(@file).should == ["Voici ", "la ", "ligne ", "une.\nQui ", "\303\250 ",
        "la ", "linea ", "due.\nAqu\303\255 ", "est\303\241 ", "la ", "l\303\255nea ",
        "tres.\nIst ", "hier ", "Linie ", "vier.\nEst\303\241 ", "aqui ", "a ",
        "linha ", "cinco.\nHere ", "is ", "line ", "six.\n"]
    ensure
      $/ = old_sep
    end
  end

  it "raises an Errno::ENOENT error when the passed file_name does not exist" do
    non_existing_file = File.dirname(__FILE__) + '/fixtures/should_not_exist.txt'
    lambda { IO.readlines(non_existing_file) }.should raise_error(Errno::ENOENT)
  end

  it "does not change $_" do
    $_ = "test"
    IO.readlines(@file)
    $_.should == "test"
  end

  it "tries to convert the passed file_name to a String using #to_str" do
    obj = mock('to_str')
    obj.stub!(:to_str).and_return(@file)
    IO.readlines(obj).should == ["Voici la ligne une.\n",
      "Qui \303\250 la linea due.\n",
      "Aqu\303\255 est\303\241 la l\303\255nea tres.\n",
      "Ist hier Linie vier.\n", "Est\303\241 aqui a linha cinco.\n",
      "Here is line six.\n"]
  end
end

describe "IO#readlines when passed [file_name, separator]" do
  before(:each) do
    @file = File.dirname(__FILE__) + '/fixtures/readlines.txt'
  end

  it "returns an Array containing lines of file_name based on the passed separator" do
    IO.readlines(@file, 'r').should == [
      "Voici la ligne une.\nQui \303\250 la linea due.\nAqu\303\255 est\303\241 la l\303\255nea tr",
      "es.\nIst hier",
      " Linie vier",
      ".\nEst\303\241 aqui a linha cinco.\nHer",
      "e is line six.\n"]
  end

  it "does not change $_" do
    $_ = "test"
    IO.readlines(@file, 'r')
    $_.should == "test"
  end

  it "returns an Array containing all paragraphs when the passed separator is an empty String" do
    para_file = File.dirname(__FILE__) + '/fixtures/paragraphs.txt'
    IO.readlines(para_file, "").should == ["This is\n\n", "an example\n\n", "of paragraphs."]
  end

  it "tries to convert the passed separator to a String using #to_str" do
    obj = mock('to_str')
    obj.stub!(:to_str).and_return("r")
    IO.readlines(@file, obj).should == [
      "Voici la ligne une.\nQui \303\250 la linea due.\nAqu\303\255 est\303\241 la l\303\255nea tr",
      "es.\nIst hier",
      " Linie vier",
      ".\nEst\303\241 aqui a linha cinco.\nHer",
      "e is line six.\n"]
  end
end
