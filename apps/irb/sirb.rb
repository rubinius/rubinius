require 'bytecode/compiler'

str = Readline.readline("irb> ")
while str
  if str
    begin
      p str.to_sexp("(irb)", 1, false)
    rescue SyntaxError => e
      puts "SyntaxError: #{e.message}"
      puts  "||  #{e.line}"
      print "||  "
      e.column.times { print " " }
      puts "^"
    end
  end
  
  str = Readline.readline("irb> ")
end