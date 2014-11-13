
c = ::RBe::Cpp::Class["Button"]
if c
  c.wrapper = true

  # GetPreferredSize
  f = c.functions("GetPreferredSize").first
  f.args.each{|a| a.out = true}

  #noimp
  %w(Perform
     SetIcon
  ).each do |name|
     c.functions(name).each{|f| f.set_option :noimp, true}
  end

  # MouseMoved
  f = c.functions("MouseMoved").first
  f.args[2].accept_nil = true
end
