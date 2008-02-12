require 'rdoc/markup/to_html'

TEMPLATE = <<-EOF
<html>
<head>
<meta http-equiv="Content-type" content="text/html; charset=utf-8" />
<title>Rubinius</title>
<link rel="stylesheet" href="styles.css" type="text/css" />
</head>
<body>
%s
</body>
<html>
EOF

formatter = RDoc::Markup::ToHtml.new

source = ARGV.shift
dest = ARGV.shift

html = formatter.convert File.read(source)

out = dest ? File.open(dest, 'w') : $stdout

out.puts TEMPLATE % html

