# All files in the 'lib' directory will be loaded
# before nanoc starts compiling.

include Nanoc3::Helpers::LinkTo

def table_of_contents
  toc = "<ul>\n"

  [ ["What is Rubinius", "/"],
    ["Getting Started", "/getting_started/"],
    ["Contributing",    "/contributing/"]
  ].each do |name, link|
    toc << "<li>#{link_to_unless_current(name, link)}</li>"
  end

  toc << "</ul>\n"
end
