shared :io_each do |cmd|
  @testfile = File.dirname(__FILE__) + '/../fixtures/gets.txt'

  @lines = ["Voici la ligne une.\n", "Qui \303\250 la linea due.\n", "\n", "\n",
      "Aqu\303\255 est\303\241 la l\303\255nea tres.\n", "Ist hier Linie vier.\n", "\n",
      "Est\303\241 aqui a linha cinco.\n", "Here is line six.\n"]

  describe "IO##{cmd}" do
    it "yields the next line of string that is separated by $/" do
      File.open(@testfile, 'r') do |f|
        i = 0
        f.send(cmd) { |line| line.should == @lines[i]; i += 1 }
      end
    end

    it "yields the entire content if the separator is nil" do
      File.open(@testfile, 'r') do |f|
        f.send(cmd, nil) { |line| line.should == @lines.join('') }
      end
    end

    # Two successive newlines in the input separate paragraphs.
    # When there are more than two successive newlines, only two are kept.
    it "yields the next paragraph if the separator's length is 0" do
      pars = ["Voici la ligne une.\nQui \303\250 la linea due.\n\n",
            "Aqu\303\255 est\303\241 la l\303\255nea tres.\nIst hier Linie vier.\n\n",
            "Est\303\241 aqui a linha cinco.\nHere is line six.\n"]

      File.open(@testfile, 'r') do |f|
        i = 0
        f.send(cmd, '') { |par| par.should == pars[i]; i += 1 }
      end
    end

    it "does not change $_" do
      File.open(@testfile, 'r') do |f|
        val = "Should not change"
        $_ = val
        f.send(cmd) { |line| }
        $_.should == val
      end
    end
  end
end
