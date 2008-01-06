module IRB
  class IrbRubinius < Irb
    def process_statements
      @scanner.each_top_level_statement do |line, line_no|
        signal_status(:IN_EVAL) do
          begin
            line.untaint
            @context.evaluate(line, line_no)
            output_value if @context.echo?
          rescue SystemExit, ThrownValue => e
            $! = e
          rescue Object => e
            
            puts "#{e.class}: #{e.message}"
            
            bt = e.backtrace

            continue = true
            bt.frames.each do |frame|
              next unless continue
              if %r!kernel/core/eval.rb!.match(frame.last)
                continue = false
                next
              end
              
              if %r!main.irb_binding!.match(frame.first)
                puts "   from #{frame[1]}"
                break
              end
              
              puts "   from #{frame[0]} at #{frame[1]}"
            end            
          end
        end
      end
    end
  end
end

IRB.conf[:IRB_CLASS] = IRB::IrbRubinius
