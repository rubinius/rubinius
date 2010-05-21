#!/usr/local/bin/ruby
#
#   save-history.rb -
#   	$Release Version: 0.9.5$
#   	$Revision: 11708 $
#   	$Date: 2007-02-12 15:01:19 -0800 (Mon, 12 Feb 2007) $
#   	by Keiju ISHITSUKAkeiju@ruby-lang.org)
#
# --
#
#
#

require "readline"

module IRB
  class Context
    def init_save_history
      hist = IRB.conf[:HISTORY_FILE]
      hist = IRB.rc_file("_history") unless hist
      if File.exist?(hist)
        open(hist) do |f|
          f.each {|l| Readline::HISTORY << l.chomp}
        end
      end

      at_exit do
        if num = IRB.conf[:SAVE_HISTORY] and (num = num.to_i) > 0
          if hf = IRB.conf[:HISTORY_FILE]
            file = File.expand_path(hf)
          end
          file = IRB.rc_file("_history") unless file

          open(file, 'w' ) do |f|
            prev_line = nil
            Readline::HISTORY.each do |line|
              unless line == prev_line
                f.puts line
              end

              prev_line = line
            end
          end
        end
      end
    end

    def save_history
      IRB.conf[:SAVE_HISTORY]
    end

    def save_history=(val)
      IRB.conf[:SAVE_HISTORY] = val
      if val
        main_context = IRB.conf[:MAIN_CONTEXT]
        main_context = self unless main_context
        main_context.init_save_history
      end
    end

    def history_file
      IRB.conf[:HISTORY_FILE]
    end

    def history_file=(hist)
      IRB.conf[:HISTORY_FILE] = hist
    end
  end
end

