
c = RBe::Cpp::Class["Invoker"]
if c
  # TODO
  %w(Target).each do |s|
    c.functions(s).each{|f| f.set_option :custom, true }
  end
end
