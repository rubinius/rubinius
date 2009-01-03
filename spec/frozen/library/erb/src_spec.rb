require 'erb'
require File.dirname(__FILE__) + '/../../spec_helper'

describe "ERB#src" do

  ruby_version_is "" ... "1.8.7" do
    it "returns the compiled ruby code" do
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

  ruby_version_is "1.8.7" do
    it "returns the compiled ruby code" do
      input = <<'END'
<ul>
<% for item in list %>
<li><%= item %></li>
<% end %>
</ul>
END

      expected = <<EOS
_erbout = ''; _erbout.concat "<ul>\\n"
;  for item in list ; _erbout.concat "\\n<li>"
; _erbout.concat(( item ).to_s); _erbout.concat "</li>\\n"
;  end ; _erbout.concat "\\n</ul>\\n"

; _erbout
EOS

      expected.chomp!
      ERB.new(input).src.should == expected
    end
  end

end
