require 'erb'
require File.dirname(__FILE__) + '/../../spec_helper'


## http://doc.loveruby.net/refm/api/view/library/erb


describe "ERB.new" do

  eruby_str = <<'END'
<ul>
<% for item in list %>
  <% if item %>
  <li><%= item %>
  <% end %>
<% end %>
</ul>
END


  ## trim_mode == 0 , '', or nil
  it "compile eRuby script into ruby code when trim mode is 0 or not specified" do
    input = eruby_str
    expected = <<'END'
_erbout = ''; _erbout.concat "<ul>\n"
 for item in list ; _erbout.concat "\n"
_erbout.concat "  ";  if item ; _erbout.concat "\n"
_erbout.concat "  <li>"; _erbout.concat(( item ).to_s); _erbout.concat "\n"
_erbout.concat "  ";  end ; _erbout.concat "\n"
 end ; _erbout.concat "\n"
_erbout.concat "</ul>\n"
_erbout
END
    expected.chomp!
    ERB.new(input).src.should == expected
    [0, '', nil].each do |trim_mode|
      ERB.new(input, nil, trim_mode).src.should == expected
    end
  end


  ## trim_mode == 1 or '>'
  it 'remove "\n" when trim_mode is 1 or \'>\'' do
    input = eruby_str
    expected = <<'END'
_erbout = ''; _erbout.concat "<ul>\n"
 for item in list 
_erbout.concat "  ";  if item 
_erbout.concat "  <li>"; _erbout.concat(( item ).to_s)
_erbout.concat "  ";  end 
 end 
_erbout.concat "</ul>\n"
_erbout
END
    expected.chomp!
    [1, '>'].each do |trim_mode|
      ERB.new(input, nil, trim_mode).src.should == expected
    end
  end


  ## trim_mode == 2 or '<>'
  it 'remove spaces at beginning of line and "\n" when trim_mode is 2 or \'<>\'' do
    input = eruby_str
    expected = <<'END'
_erbout = ''; _erbout.concat "<ul>\n"
 for item in list 
_erbout.concat "  ";  if item ; _erbout.concat "\n"
_erbout.concat "  <li>"; _erbout.concat(( item ).to_s); _erbout.concat "\n"
_erbout.concat "  ";  end ; _erbout.concat "\n"
 end 
_erbout.concat "</ul>\n"
_erbout
END
    expected.chomp!
    [2, '<>'].each do |trim_mode|
      ERB.new(input, nil, trim_mode).src.should == expected
    end
  end


  ## trim_mode == '-'
  it "removes spaces arount '<%- -%>' when trim_mode is '-'" do
    #input = eruby_str.gsub(/<%/, '<%-')
    input = <<'END'
<ul>
<%- for item in list -%>
  <%- if item -%>
  <li><%= item -%>
  <%- end -%>
<%- end -%>
</ul>
END
    expected = <<'END'
_erbout = ''; _erbout.concat "<ul>\n"
 for item in list 
 if item 
_erbout.concat "  <li>"; _erbout.concat(( item ).to_s)
 end 
 end 
_erbout.concat "</ul>\n"
_erbout
END
    expected.chomp!
    ['-'].each do |trim_mode|
      ERB.new(input, nil, trim_mode).src.should == expected
    end
  end


  ## trim_mode == '-'  #2
  it "not support '<%-= expr %> even when trim_mode is '-'" do
    input = <<'END'
<p>
  <%= expr -%>
  <%-= expr -%>
</p>
END
    expected = <<'END'
_erbout = ''; _erbout.concat "<p>\n"
_erbout.concat "  "; _erbout.concat(( expr ).to_s)
= expr 
_erbout.concat "</p>\n"
_erbout
END
    expected.chomp!
    ['-'].each do |trim_mode|
      ERB.new(input, nil, trim_mode).src.should == expected
    end
  end


  erubystr2 = <<'END'
<ul>
%for item in list
%  if item
  <li><%= item %>
  <% end %>
<% end %>
</ul>
%%%
END


  ## trim_mode == '%'
  it "regard lines starting with '%' as '<% ... %>' when trim_mode is '%'" do
    input = erubystr2
    expected = <<'END'
_erbout = ''; _erbout.concat "<ul>\n"
for item in list
  if item
_erbout.concat "  <li>"; _erbout.concat(( item ).to_s); _erbout.concat "\n"
_erbout.concat "  ";  end ; _erbout.concat "\n"
 end ; _erbout.concat "\n"
_erbout.concat "</ul>\n"
_erbout.concat "%%\n"
_erbout
END
    expected.chomp!
    ['%'].each do |trim_mode|
      ERB.new(input, nil, trim_mode).src.should == expected
    end
  end


  ## trim_mode == '%>'
  it "regard lines starting with '%' as '<% ... %>' and remove \"\\n\" when trim_mode is '%>'" do
    input = erubystr2
    expected = <<'END'
_erbout = ''; _erbout.concat "<ul>\n"
for item in list
  if item
_erbout.concat "  <li>"; _erbout.concat(( item ).to_s)
_erbout.concat "  ";  end 
 end 
_erbout.concat "</ul>\n"
_erbout.concat "%%\n"
_erbout
END
    expected.chomp!
    ['%>'].each do |trim_mode|
      ERB.new(input, nil, trim_mode).src.should == expected
    end
  end


  ## trim_mode == '%<>'
  it "regard lines starting with '%' as '<% ... %>' and remove \"\\n\" when trim_mode is '%<>'" do
    input = erubystr2
    expected = <<'END'
_erbout = ''; _erbout.concat "<ul>\n"
for item in list
  if item
_erbout.concat "  <li>"; _erbout.concat(( item ).to_s); _erbout.concat "\n"
_erbout.concat "  ";  end ; _erbout.concat "\n"
 end 
_erbout.concat "</ul>\n"
_erbout.concat "%%\n"
_erbout
END
    expected.chomp!
    ['%<>'].each do |trim_mode|
      ERB.new(input, nil, trim_mode).src.should == expected
    end
  end


  ## trim_mode == '%-'
  it "regard lines starting with '%' as '<% ... %>' and spaces around '<%- -%>' when trim_mode is '%-'" do
    input = <<'END'
<ul>
%for item in list
%  if item
  <li><%= item -%>
  <%- end -%>
<%- end -%>
</ul>
%%%
END
    expected = <<'END'
_erbout = ''; _erbout.concat "<ul>\n"
for item in list
  if item
_erbout.concat "  <li>"; _erbout.concat(( item ).to_s)
 end 
 end 
_erbout.concat "</ul>\n"
_erbout.concat "%%\n"
_erbout
END
    expected.chomp!
    ['%-'].each do |trim_mode|
      ERB.new(input, nil, trim_mode).src.should == expected
    end
  end


  ## safe_level
  #it "set savel level" do
  #  input = ''
  #  safe_level = 4
  #  erb = ERB.new(input, safe_level)
  #end


  ## eoutvar
  it "change '_erbout' variable name" do
    input = eruby_str
    expected = <<'END'
buf = ''; buf.concat "<ul>\n"
 for item in list ; buf.concat "\n"
buf.concat "  ";  if item ; buf.concat "\n"
buf.concat "  <li>"; buf.concat(( item ).to_s); buf.concat "\n"
buf.concat "  ";  end ; buf.concat "\n"
 end ; buf.concat "\n"
buf.concat "</ul>\n"
buf
END
    expected.chomp!
    ERB.new(input, nil, nil, 'buf').src.should == expected
  end


  ## <%# ... %>
  it "ignore '<%# ... %>'" do
    input = <<'END'
<%# for item in list %>
<b><%#= item %></b>
<%# end %>
END
    expected = <<'END'
_erbout = ''; _erbout.concat "\n"
_erbout.concat "<b>"; _erbout.concat "</b>\n"
_erbout.concat "\n"
_erbout
END
    expected.chomp!
    ERB.new(input).src.should == expected
    #
    expected = <<'END'
_erbout = ''
_erbout.concat "<b>"; _erbout.concat "</b>\n"

_erbout
END
    expected.chomp!
    ERB.new(input, nil, '<>').src.should == expected
  end


  ## <%% ... %%>
  it "convert '<%% ... %%>' into '<% ... %>'" do
    input = <<'END'
<%% for item in list %>
<b><%%= item %></b>
<%% end %%>
END
    expected = <<'END'
_erbout = ''; _erbout.concat "<% for item in list %>\n"
_erbout.concat "<b><%= item %></b>\n"
_erbout.concat "<% end %%>\n"
_erbout
END
    expected.chomp!
    ERB.new(input).src.should == expected
  end


end
