begin
  require 'ext/readline/readline'

  Readline::HISTORY.extend Enumerable

  module Readline
    def self.readline(prompt="", add_hist=true)
      perform_readline(prompt, add_hist)
    end

    def readline(prompt="", add_hist=true)
      Readline.perform_readline(prompt, add_hist)
    end
  end
rescue
  require 'readline-native'
end
