require 'readline/ext/readline'

module Readline
  def self.readline(prompt="", add_hist=true)
    perform_readline(prompt, add_hist)
  end

  def readline(prompt="", add_hist=true)
    Readline.perform_readline(prompt, add_hist)
  end
end

Readline::HISTORY.extend Enumerable
