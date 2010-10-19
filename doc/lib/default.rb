# All files in the 'lib' directory will be loaded
# before nanoc starts compiling.

include Nanoc3::Helpers::LinkTo

def table_of_contents
  toc = "<ul>\n"

  [ ["What is Rubinius",  "/"],
    ["Getting Started",   "/getting_started/"],
    ["Common Problems",   "/common_problems/"],
    ["Contributing",      "/contributing/"]
  ].each do |name, link|
    toc << "<li>#{link_to_unless_current(name, link)}</li>\n"
  end

  toc << "</ul>\n"
  toc << "<h3>Tools</h3>\n"
  toc << "<ul>\n"

  [ ["Profiler", "/tools/profiler/"],
    ["Debugger", "/tools/debugger/"],
  ].each do |name, link|
    toc << "<li>#{link_to_unless_current(name, link)}</li>\n"
  end

  toc << "</ul>\n"
  toc << "<h3>Developing</h3>\n"
  toc << "<ul>\n"

  [ ["Specs",             "/dev/specs/"],
    ["Build System",      "/dev/build_system/"],
    ["Multiverse",        "/dev/multiverse/"],
    ["Bootstrapping",     "/dev/bootstrapping/"],
    ["Bytecode Compiler", "/dev/compiler/"],
    ["Style Guide",       "/dev/style_guide/"],
    ["Reading List",      "/dev/reading_list/"],
    ["Glossary",          "/glossary/"]
  ].each do |name, link|
    toc << "<li>#{link_to_unless_current(name, link)}</li>\n"
  end

  toc << "</ul>\n"
  toc << "<h3>Howto</h3>\n"
  toc << "<ul>\n"

  [ ["Write a Ticket",      "/howto/write_a_ticket/"],
    ["Write a Ruby spec",   "/howto/write_a_ruby_spec/"],
    ["Fix a Failing Spec",  "/howto/fix_a_failing_spec/"],
  ].each do |name, link|
    toc << "<li>#{link_to_unless_current(name, link)}</li>\n"
  end

  toc << "</ul>\n"
end
