# -*- encoding: utf-8 -*-

module IOSpecs
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

  def self.io_fixture(name, mode="r:utf-8")
    path = fixture __FILE__, name
    if File.exists? path
      File.open path, fmode(mode)
    else
      File.open name, fmode(mode)
    end
  end

  def self.closed_file
    File.open(fixture(__FILE__, "lines.txt"), fmode("r:utf-8")) { |f| f }
  end

  class NotConvertable
    def to_io
      raise TypeError
    end
  end
end
