require File.dirname(__FILE__) + '/../../spec_helper'

describe "IO#gets" do
  @testfile = File.dirname(__FILE__) + '/fixtures/gets.txt'
  @createfile = File.dirname(__FILE__) + '/fixtures/i_exit'

  after :each do
    File.delete @createfile if File.exists?(@createfile)
  end

  @lines = ["Voici la ligne une.\n", "Qui \303\250 la linea due.\n", "\n", "\n",
        "Aqu\303\255 est\303\241 la l\303\255nea tres.\n", "Ist hier Linie vier.\n", "\n",
        "Est\303\241 aqui a linha cinco.\n", "Here is line six.\n"]

  it "returns the next line of string that were separated by $/" do
    File.open(@testfile, 'r') do |f|
      @lines.each {|line| line.should == f.gets}
    end
  end

  it "assigns the returned line to $_" do
    File.open(@testfile, 'r') do |f|
      @lines.each do |line|
        f.gets
        $_.should == line
      end
    end
  end

  it "returns nil if called at the end of the stream" do
    File.open(@testfile, 'r') do |f|
      @lines.length.times { f.gets }
      f.gets.should == nil
    end
  end

  it "returns the entire content if the separator is nil" do
    File.open(@testfile, 'r') do |f|
      f.gets(nil).should == @lines.join('')
    end
  end

  # Two successive newlines in the input separate paragraphs.
  # When there are more than two successive newlines, only two are kept.
  it "returns the next paragraph if the separator's length is 0" do
    pars = ["Voici la ligne une.\nQui \303\250 la linea due.\n\n",
          "Aqu\303\255 est\303\241 la l\303\255nea tres.\nIst hier Linie vier.\n\n",
          "Est\303\241 aqui a linha cinco.\nHere is line six.\n"]
    File.open(@testfile, 'r') do |f|
      pars.each {|par| par.should == f.gets("")}
    end
  end

  # This could probably be added to the previous test using pos, but right now this doesn't
  # pass and the previous test does.
  it "reads until the beginning of the next paragraph when the separator's length is 0" do
    # Leverage the fact that there are three newlines between the first and second paragraph
    File.open(@testfile, 'r') do |f|
      f.gets('')

      # This should return 'A', the first character of the next paragraph, not $/
      f.read(1).should == 'A'
    end
  end

  it "raises IOError if the stream is not opened for reading" do
    should_raise(IOError) do
      File.open(@createfile, 'a') {|f| f.gets}
    end

    should_raise(IOError) do
      File.open(@createfile, 'w') {|f| f.gets}
    end
  end
end
