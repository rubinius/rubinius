class Rubinius::Debugger
  module Display
    def info(str)
      puts "| #{str}"
    end

    def display(str)
      puts "=> #{str}"
    end

    def crit(str)
      puts "[CRITICAL] #{str}"
    end

    def error(str)
      puts "* #{str}"
    end

    def section(str)
      puts "==== #{str} ===="
    end

    def ask(str)
      Readline.readline("| #{str}")
    end
  end
end
