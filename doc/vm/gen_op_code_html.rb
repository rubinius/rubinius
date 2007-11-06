require 'yaml'
require 'erb'
require 'pp'

include ERB::Util

class OpCode
  def initialize(op_code)
    @op_code = op_code
  end
  
  def method_missing(method, *args)
    @op_code[method.to_s]
  end
  
  def to_s
    pp @op_code
  end
end


def create_html(file_name, template, bind)
  # Create documentation page for this op code
  File.open(file_name, 'w') do |f|
    html = template.result(bind)
    f.puts html
  end
end

# Supports VERY simple formatting instructions in a description. At present, 
# _italics_ is converted to italics, and *bold text* is converted to bold.
# Links are supported via "link":url, and line breaks are converted to <p>.
def markup_html(str)
  html = str.gsub(/_((\w|\s)+)_/){|m| "<i>#{$1}</i>" }.gsub(/\*((\w|\s)+)\*/) {|m| "<b>#{$1}</b>"}
  html = html.gsub(/"(.+)":((\w|\/)+(\.\w+)?)/){|m| "<a href=\"#{$2}\">#{$1}</a>"}
  %Q{<p>#{html.gsub("\n","</p><p>")}</p>}
end

# Outputs a representation of the stack
def markup_stack(ary)
  html = "<table>"
  ary.each do |i|
    html << "<tr><td>"
    case i
    when String
      html << html_escape(i)
    when Array
      html << "[#{html_escape i.join(', ')}]"
    when Hash
      html << "{"
      html << (i.map do |key,val|
        html_escape "#{key}=>#{val}"
      end.join(', '))
      html << "}"
    end
  end
  html << "</td></tr></table>"
  html
end

op_code_template = ERB.new(File.read('op_code_template.html.erb'),nil,'<>')
index_template = ERB.new(File.read('op_codes_index.html.erb'), nil, '<>')

op_codes = []
Dir.glob('op_codes/*.yaml').each do |f|
  YAML::load_documents(File.open(f)) do |op|
    op_code = OpCode.new(op)
    op_codes << op_code
    
    # Create documentation page for this op code
    create_html("op_codes/#{op_code.mnemonic}.html", op_code_template, binding) if ARGV.empty? or ARGV.include? op_code.mnemonic
  end
end

# Create index page
create_html("op_codes_index.html", index_template, binding)