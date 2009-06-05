# encoding: utf-8
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
  
  def self.gets_fixtures
    File.dirname(__FILE__) + '/gets.txt'
  end
  
  def self.gets_output
    File.dirname(__FILE__) + '/gets_output.txt'
  end
  
  def self.closed_file
    File.open(File.dirname(__FILE__) + '/gets.txt', 'r') { |f| f }
  end

  def self.closed_io
    File.open(gets_fixtures, 'r') { |file|
      IO.open(file.fileno, 'r') { |io|
        io
      }
    }
  end

end
