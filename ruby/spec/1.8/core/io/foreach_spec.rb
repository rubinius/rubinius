require File.dirname(__FILE__) + '/../../spec_helper'

describe "IO::foreach" do
  it "yields a sequence of Strings that were separated by $/" do
    lines = []
    IO::foreach(File.dirname(__FILE__) + '/fixtures/readlines.txt') do |line|
      lines << line
    end
    lines.should == ["Voici la ligne une.\n", "Qui \303\250 la linea due.\n", 
                     "Aqu\303\255 est\303\241 la l\303\255nea tres.\n", "Ist hier Linie vier.\n", 
                     "Est\303\241 aqui a linha cinco.\n", "Here is line six.\n"]
  end  
  it "yields a sequence of Strings that were separated by r" do
    lines = []
    IO::foreach(File.dirname(__FILE__) + '/fixtures/readlines.txt','r') do |line|
      lines << line
    end
    lines.should == ["Voici la ligne une.\nQui \303\250 la linea due.\nAqu\303\255 est\303\241 la l\303\255nea tr",
                     "es.\nIst hier"," Linie vier",".\nEst\303\241 aqui a linha cinco.\nHer","e is line six.\n"]
  end
end
