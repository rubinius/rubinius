require 'erb'
require File.dirname(__FILE__) + '/../../spec_helper'

describe "ERB#src" do

  it "return compiled ruby code" do
    input = <<'END'
<ul>
<% for item in list %>
<li><%= item %></li>
<% end %>
</ul>
END
    expected = <<'END'
_erbout = ''; _erbout.concat "<ul>\n"
 for item in list ; _erbout.concat "\n"
_erbout.concat "<li>"; _erbout.concat(( item ).to_s); _erbout.concat "</li>\n"
 end ; _erbout.concat "\n"
_erbout.concat "</ul>\n"
_erbout
END
    expected.chomp!
    ERB.new(input).src.should == expected
  end

end
