register_dataform "Bytecode" do |relation|
  comp = Compile;
  node = comp.convert_sexp s(:snippit, relation["Sexp"])

  node.bytoced actual
  actual.should_with_message relation["Bytecode"], "dataform ...."
end
