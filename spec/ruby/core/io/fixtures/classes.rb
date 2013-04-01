# -*- encoding: utf-8 -*-

module IOSpecs
  class SubIO < IO
  end

  def self.collector
    Proc.new { |x| ScratchPad << x }
  end

  def self.lines
    [ "Voici la ligne une.\n",
      "Qui \303\250 la linea due.\n",
      "\n",
      "\n",
      "Aqu\303\255 est\303\241 la l\303\255nea tres.\n",
      "Ist hier Linie vier.\n",
      "\n",
      "Est\303\241 aqui a linha cinco.\n",
      "Here is line six.\n" ]
  end

  def self.lines_limit
    [ "Voici la l",
      "igne une.\n",
      "Qui è la ",
      "linea due.",
      "\n",
      "\n",
      "\n",
      "Aquí está",
      " la línea",
      " tres.\n",
      "Ist hier L",
      "inie vier.",
      "\n",
      "\n",
      "Está aqui",
      " a linha c",
      "inco.\n",
      "Here is li",
      "ne six.\n" ]
  end

  def self.lines_space_separator_limit
    [ "Voici ",
      "la ",
      "ligne ",
      "une.\nQui ",
      "è ",
      "la ",
      "linea ",
      "due.\n\n\nAqu",
      "í ",
      "está ",
      "la ",
      "línea ",
      "tres.\nIst ",
      "hier ",
      "Linie ",
      "vier.\n\nEst",
      "á ",
      "aqui ",
      "a ",
      "linha ",
      "cinco.\nHer",
      "e ",
      "is ",
      "line ",
      "six.\n" ]
  end

  def self.lines_r_separator
    [ "Voici la ligne une.\nQui \303\250 la linea due.\n\n\n" \
      "Aqu\303\255 est\303\241 la l\303\255nea tr",
      "es.\nIst hier",
      " Linie vier",
      ".\n\nEst\303\241 aqui a linha cinco.\nHer",
      "e is line six.\n" ]
  end

  def self.lines_empty_separator
    [ "Voici la ligne une.\nQui \303\250 la linea due.\n\n",
      "Aqu\303\255 est\303\241 la l\303\255nea tres.\nIst hier Linie vier.\n\n",
      "Est\303\241 aqui a linha cinco.\nHere is line six.\n" ]
  end

  def self.lines_space_separator
    [ "Voici ", "la ", "ligne ", "une.\nQui ",
      "\303\250 ", "la ", "linea ", "due.\n\n\nAqu\303\255 ",
      "est\303\241 ", "la ", "l\303\255nea ", "tres.\nIst ",
      "hier ", "Linie ", "vier.\n\nEst\303\241 ", "aqui ", "a ",
      "linha ", "cinco.\nHere ", "is ", "line ", "six.\n" ]
  end

  def self.lines_arbitrary_separator
    [ "Voici la ligne une.\nQui \303\250",
      " la linea due.\n\n\nAqu\303\255 est\303\241 la l\303\255nea tres.\n" \
      "Ist hier Linie vier.\n\nEst\303\241 aqui a linha cinco.\nHere is line six.\n" ]
  end

  def self.paragraphs
    [ "Voici la ligne une.\nQui \303\250 la linea due.\n\n",
      "Aqu\303\255 est\303\241 la l\303\255nea tres.\nIst hier Linie vier.\n\n",
      "Est\303\241 aqui a linha cinco.\nHere is line six.\n" ]
  end

  # Creates an IO instance for an existing fixture file. The
  # file should obviously not be deleted.
  def self.io_fixture(name, options_or_mode="r:utf-8")
    path = fixture __FILE__, name
    name = path if File.exists? path
    new_io name, options_or_mode
  end

  # Returns a closed instance of IO that was opened to reference
  # a fixture file (i.e. the IO instance was perfectly valid at
  # one point but is now closed).
  def self.closed_io
    io = io_fixture "lines.txt"
    io.close
    io
  end

  # Creates a pipe-based IO fixture containing the specified
  # contents
  def self.pipe_fixture(content)
    source, sink = IO.pipe
    sink.write content
    sink.close
    source
  end

  # Defines +method+ on +obj+ using the provided +block+. This
  # special helper is needed for e.g. IO.open specs to avoid
  # mock methods preventing IO#close from running.
  def self.io_mock(obj, method, &block)
    obj.singleton_class.send(:define_method, method, &block)
  end

  module CopyStream
    def self.from=(from)
      @from = from
    end

    def self.from
      @from
    end

    def self.flush=(flush)
      @flush = true
    end

    def self.flush?
      @flush
    end
  end
end
