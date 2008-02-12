require 'rdoc/rdoc'
require 'rdoc/generator'
require 'rdoc/markup/to_html_hyperlink'
require 'kernel/core/iseq'

class RDoc::Generator::Method
  def path
    "op_codes/#{name}.html"
  end
end

OP_CODE_TEMPLATE = <<-EOF
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd">
<html>
<head>
<meta http-equiv="Content-type" content="text/html; charset=utf-8" />
<title>Rubinius Virtual Machine OpCode: %s</title>
<link rel="stylesheet" href="../styles.css" type="text/css" />
</head>
<body>
<div id="%s">
%s
</div>
</body>
</html>
EOF

TOC_TEMPLATE = <<-EOF
<html>
<head>
<meta http-equiv="Content-type" content="text/html; charset=utf-8" />
<title>Rubinius Virtual Machine OpCodes</title>
<link rel="stylesheet" href="styles.css" type="text/css" />
</head>

<body>
<ul id="chapters">
<li><a href="intro.html" target="op_code">Intro</a></li>
<li><a href="shotgun.html" target="op_code">Shotgun</a></li>
<li><a href="rubinius_vs_mri.html" target="op_code">Rubinius vs MRI</a></li>
<li><a href="concurrency.html" target="op_code">Concurrency</a></li>
<li><a href="vm_interfaces.html" target="op_code">Interfacing to Rubinius</a></li>
<li><a href="rbc_files.html" target="op_code">Rubinius Compiled Files</a></li>
</ul>

<h3>OpCodes</h3>

<ul id="op_codes" class="toc_subsection">
%s
</ul>
</body>
<html>
EOF

class ToRubiniusOpCode < RDoc::Markup::ToHtmlHyperlink

  def initialize(*args)
    super

    @markup.add_special(/<\/?code>/, :CODE)

    @level = nil
    @name = nil
    @stack = nil
  end

  def accept_list_end(am, fragment)
    if @level and fragment.level > @level
      @stack << fragment
      return
    elsif fragment.level == @level then
      @stack << fragment
      make_stack am, @name, @stack
      @stack = nil
      @level = nil
    else
      super
    end
  end

  def accept_list_item(am, fragment)
    if @level and fragment.level >= @level then
      @stack << fragment
      return
    elsif fragment.param =~ /^Stack/ then
      @stack = []
      @name = fragment.param
      @level = fragment.level + 1
    end

    super
  end

  def accept_list_start(am, fragment)
    if @level and fragment.level >= @level
      @stack << fragment
    else
      super
    end
  end

  def gen_url(url, text)
    super
  end

  ##
  # This is a hack around a misfeature in RDoc for labeled list handling.

  def handle_special_CODE(special)
    ''
  end

  def make_stack(am, name, stack)
    level = stack.first.level

    stack.each_with_index do |fragment, i|
      case fragment
      when RDoc::Markup::ListStart then
        @res << "<table class=\"stack\">\n"
      when RDoc::Markup::ListEnd then
        @res << "</table>\n"
      when RDoc::Markup::ListItem then
        css_class = (i == stack.length - 2) ? ' class="rest"' : nil
        @res << "<tr><td#{css_class}>#{fragment.txt}</td></tr>\n"
      else
        raise "Unknown fragment #{fragment.inspect}"
      end
    end
  end

end

options = RDoc::Options.new({})
options.generator = nil
options.files = ['shotgun/lib/instructions.rb']
options.inline_source = true

rdoc = RDoc::RDoc.new

toplevels = rdoc.parse_files options

files, classes = RDoc::Generator::Context.build_indicies toplevels, options

formatter = ToRubiniusOpCode.new '', nil, true

toc = []

classes.each do |klass|
  formatter.context = klass

  klass.methods.each do |method|
    str = method.context.comment

    source = method.context.token_stream.find do |ts|
      RubyToken::TkSTRING === ts
    end

    source = source.text[1..-2]

    source = source.split("\\n").map do |line|
      "      #{line}"
    end.join("\n").rstrip

    if str =~ /^(?>\s*)[^\#]/ then
      content = str
    else
      content = str.gsub(/^\s*(#+)/) { $1.tr '#', ' ' }
    end

    bytecode = InstructionSet[method.name.intern].bytecode rescue nil

    next if bytecode.nil?

    content.gsub!(/\[Format\](\s+)(\\\w+)/m, '[Format]\1*\2*')

    content = <<-EOF
  = \\#{method.name}

  [Byte code] 0x#{bytecode.to_s 16}
#{content}
  [Source]
    <code>
#{source}
    </code>
    EOF

    html = OP_CODE_TEMPLATE % [
      method.name,
      method.name,
      formatter.convert(content)
    ]

    file = File.join 'doc', 'vm', 'op_codes', "#{method.name}.html"
    File.open file, 'w' do |fp|
      fp.puts html
    end

    toc << method.name
  end
end

toc = toc.sort.map do |name|
  "<li><a href=\"op_codes/#{name}.html\">#{name}</a></li>"
end.join "\n"

toc_path = File.join 'doc', 'vm', 'toc.html'
toc_html = TOC_TEMPLATE % [toc]

File.open toc_path, 'w' do |fp|
  fp.puts toc_html
end

