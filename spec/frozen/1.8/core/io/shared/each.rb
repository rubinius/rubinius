require File.dirname(__FILE__) + '/../fixtures/classes'

shared :io_each do |cmd|
  describe "IO##{cmd} when passed no arguments" do
    before(:each) do
      @io = File.open(IOSpecs.gets_fixtures)
    end
    
    after(:each) do
      @io.close
    end
    
    it "yields each line to the passed block" do
      seen = []
      @io.send(cmd) {|s| seen << s }
      seen.should == ["Voici la ligne une.\n",
        "Qui \303\250 la linea due.\n",
        "\n",
        "\n", "Aqu\303\255 est\303\241 la l\303\255nea tres.\n",
        "Ist hier Linie vier.\n",
        "\n",
        "Est\303\241 aqui a linha cinco.\n",
        "Here is line six.\n"]
    end
    
    it "yields each line starting from the current position" do
      seen = []
      @io.pos = 40
      @io.send(cmd) {|s| seen << s }
      seen.should == ["\n", "\n", "\n", "Aqu\303\255 est\303\241 la l\303\255nea tres.\n", "Ist hier Linie vier.\n", "\n", "Est\303\241 aqui a linha cinco.\n", "Here is line six.\n"]
    end

    it "does not change $_" do
      $_ = "test"
      @io.send(cmd) { |s| s}
      $_.should == "test"
    end

    it "uses $/ as the default line seperator" do
      seen = []
      begin
        old_rs, $/ = $/, " "
        @io.send(cmd) {|s| seen << s }
        seen.should == ["Voici ", "la ", "ligne ", "une.\nQui ", "\303\250 ",
          "la ", "linea ", "due.\n\n\nAqu\303\255 ", "est\303\241 ", "la ",
          "l\303\255nea ", "tres.\nIst ", "hier ", "Linie ", "vier.\n\nEst\303\241 ",
          "aqui ", "a ", "linha ", "cinco.\nHere ", "is ", "line ", "six.\n"]
      ensure
        $/ = old_rs
      end
    end

    it "returns self" do
      @io.send(cmd) {|l| l }.should equal(@io)
    end
    
    ruby_version_is "" ... "1.8.7" do
      it "yields a LocalJumpError when passed no block" do
        lambda { @io.send(cmd) }.should raise_error(LocalJumpError)
      end
    end

    ruby_version_is "1.8.7" do
      it "returns an Enumerator when passed no block" do
        enum = @io.send(cmd)
        enum.instance_of?(Enumerable::Enumerator).should be_true
        
        seen = []
        enum.each { |b| seen << b }
        seen.should == ["Voici la ligne une.\n",
          "Qui \303\250 la linea due.\n",
          "\n",
          "\n", "Aqu\303\255 est\303\241 la l\303\255nea tres.\n",
          "Ist hier Linie vier.\n",
          "\n",
          "Est\303\241 aqui a linha cinco.\n",
          "Here is line six.\n"]
      end
    end
  end

  describe "IO##{cmd} when passed [seperator]" do
    before(:each) do
      @io = File.open(IOSpecs.gets_fixtures)
    end
    
    after(:each) do
      @io.close
    end

    it "uses the passed argument as the line seperator" do
      seen = []
      @io.send(cmd, " ") {|s| seen << s}
      seen.should == ["Voici ", "la ", "ligne ", "une.\nQui ", "\303\250 ", "la ",
        "linea ", "due.\n\n\nAqu\303\255 ", "est\303\241 ", "la ", "l\303\255nea ",
        "tres.\nIst ", "hier ", "Linie ", "vier.\n\nEst\303\241 ", "aqui ", "a ",
        "linha ", "cinco.\nHere ", "is ", "line ", "six.\n"]
    end
    
    it "does not change $_" do
      $_ = "test"
      @io.send(cmd, " ") { |s| s}
      $_.should == "test"
    end

    it "returns self" do
      @io.send(cmd) {|l| l }.should equal(@io)
    end

    it "tries to convert the passed seperator to a String using #to_str" do
      obj = mock("to_str")
      obj.stub!(:to_str).and_return(" ")
      
      seen = []
      @io.send(cmd, obj) { |l| seen << l }
      seen.should == ["Voici ", "la ", "ligne ", "une.\nQui ", "\303\250 ", "la ",
        "linea ", "due.\n\n\nAqu\303\255 ", "est\303\241 ", "la ", "l\303\255nea ",
        "tres.\nIst ", "hier ", "Linie ", "vier.\n\nEst\303\241 ", "aqui ", "a ",
        "linha ", "cinco.\nHere ", "is ", "line ", "six.\n"]
    end
    
    it "checks whether the passed seperator responds to #to_str" do
      obj = mock('method_missing to_str')
      obj.should_receive(:respond_to?).any_number_of_times.with(:to_str).and_return(true)
      obj.should_receive(:method_missing).any_number_of_times.with(:to_str).and_return(" ")

      seen = []
      @io.send(cmd, obj) { |l| seen << l }
      seen.should == ["Voici ", "la ", "ligne ", "une.\nQui ", "\303\250 ", "la ",
        "linea ", "due.\n\n\nAqu\303\255 ", "est\303\241 ", "la ", "l\303\255nea ",
        "tres.\nIst ", "hier ", "Linie ", "vier.\n\nEst\303\241 ", "aqui ", "a ",
        "linha ", "cinco.\nHere ", "is ", "line ", "six.\n"]
    end
    
    it "yields self's content starting from the current position when the passed seperator is nil" do
      seen = []
      @io.pos = 100
      @io.send(cmd, nil) {|s| seen << s}
      seen.should == ["qui a linha cinco.\nHere is line six.\n"]
    end
    
    it "yields each paragraph when passed an empty String as seperator" do
      seen = []
      para_file = File.dirname(__FILE__) + '/../fixtures/paragraphs.txt'
      File.open(para_file) do |io|
        io.send(cmd, "") {|s| seen << s}
      end
      seen.should == ["This is\n\n", "an example\n\n", "of paragraphs."]
    end
  end

  describe "IO##{cmd} when self is not readable" do
    it "raises an IOError" do
      # method must have a block in order to raise the IOError.
      # MRI 1.8.7 returns enumerator if block is not provided.
      # See [ruby-core:16557].
      lambda { IOSpecs.closed_file.send(cmd){} }.should raise_error(IOError)
    end
  end
end
