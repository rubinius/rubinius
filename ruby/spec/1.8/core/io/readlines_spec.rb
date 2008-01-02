require File.dirname(__FILE__) + '/../../spec_helper'

describe "IO#readlines" do
  it "returns an Array of Strings that were separated by $/" do
    File.open(File.dirname(__FILE__) + '/fixtures/readlines.txt', 'r') do |f|
      f.readlines.should == ["Voici la ligne une.\n", "Qui \303\250 la linea due.\n", 
          "Aqu\303\255 est\303\241 la l\303\255nea tres.\n", "Ist hier Linie vier.\n", 
          "Est\303\241 aqui a linha cinco.\n", "Here is line six.\n"]
    end
  end
  it "returns an Array of Strings that were separated by r" do
    File.open(File.dirname(__FILE__) + '/fixtures/readlines.txt', 'r') do |f|
      f.readlines('r').should == ["Voici la ligne une.\nQui \303\250 la linea due.\nAqu\303\255 est\303\241 la l\303\255nea tr",
                                  "es.\nIst hier"," Linie vier",".\nEst\303\241 aqui a linha cinco.\nHer","e is line six.\n"]
    end
  end
end

describe "IO::readlines" do
  it "returns an Array of Strings that were separated by $/" do
    lines = IO::readlines(File.dirname(__FILE__) + '/fixtures/readlines.txt')
    lines.should == ["Voici la ligne une.\n", "Qui \303\250 la linea due.\n", 
                     "Aqu\303\255 est\303\241 la l\303\255nea tres.\n", "Ist hier Linie vier.\n", 
                     "Est\303\241 aqui a linha cinco.\n", "Here is line six.\n"]
  end
  it "returns an Array of Strings that were separated by r" do
    lines = IO::readlines(File.dirname(__FILE__) + '/fixtures/readlines.txt','r')
    lines.should == ["Voici la ligne une.\nQui \303\250 la linea due.\nAqu\303\255 est\303\241 la l\303\255nea tr",
                     "es.\nIst hier"," Linie vier",".\nEst\303\241 aqui a linha cinco.\nHer","e is line six.\n"]
  end
end
