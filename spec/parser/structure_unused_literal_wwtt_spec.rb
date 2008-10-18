def test_case
{"RawParseTree"=>[:module, :Graffle, [:scope]],
 "Ruby"=>"\"prevent the above from infecting rdoc\"\n\nmodule Graffle\nend",
 "ParseTree"=>s(:module, :Graffle, s(:scope)),
 "Ruby2Ruby"=>"module Graffle\nend"}
end
