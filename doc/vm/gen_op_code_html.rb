require 'erb'
require 'op_code_info'


module OpCode
  class HTMLEmitter
    include ERB::Util
  
    def create_index_html(op_codes, template)
      File.open("op_codes_index.html", 'w') do |f|
        html = template.result(binding)
        f.puts html
      end
    end

    def create_opcode_html(op_code, template)
      # Create documentation page for this op code
      file_name = "op_codes/#{op_code.mnemonic.to_s}.html"
      File.open(file_name, 'w') do |f|
        html = template.result(binding)
        f.puts html
      end
    end

    # Supports VERY simple formatting instructions in a description. At present,
    # _italics_ is converted to italics, and *bold text* is converted to bold.
    # Links are supported via "link":url, and line breaks are converted to <p>.
    def markup_html(str)
      html = str.gsub(/(^|\W)_(\S([^_]|(_\w))*)_/){|m| "#{$1}<i>#{$2}</i>" }.gsub(/\*((\w|\s)+?)\*/) {|m| "<b>#{$1}</b>"}
      html = str.gsub(/@((\w|\s)+?)@/) {|m| "<code>#{$1}</code>"}
      html = html.gsub(/"(.+?)":((\w|\/)+(\.\w+)?)/){|m| "<a href=\"#{$2}\">#{$1}</a>"}
      %Q{<p>#{html.gsub("\n","</p><p>")}</p>}
    end

    # Outputs a representation of the stack
    def markup_stack(ary)
      html = "<table>"
      ary.each do |i|
        html << "<tr><td>"
        case i
        when Array
          html << "[#{html_escape i.join(', ')}]"
        when Hash
          html << "{"
          html << (i.map do |key,val|
            html_escape "#{key}=>#{val}"
          end.join(', '))
          html << "}"
        else
          html << html_escape(i)
        end
      end
      html << "</td></tr></table>"
      html
    end
  end
end

op_code_template = ERB.new(File.read('op_code_template.html.erb'),nil,'<>')
index_template = ERB.new(File.read('op_codes_index.html.erb'), nil, '<>')

op_codes = []
emitter = OpCode::HTMLEmitter.new
OpCode::Info.op_codes.each do |op|
  begin
    op_code = OpCode::Info.new(op)
    op_codes << op_code
    emitter.create_opcode_html(op_code, op_code_template) if ARGV.empty? or ARGV.include? op.to_s
  rescue Exception => ex
    puts "An exception occurred while processing op code '#{op_code}'"
    raise ex
  end
end


# Create index page
emitter.create_index_html(op_codes, index_template)
#create_html("op_codes_index.html", index_template, binding)
